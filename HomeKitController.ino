#include "Utilities.h"
#include "Button.h"
#include <Encoder.h>

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
Adafruit_MQTT_Subscribe statusFeed = Adafruit_MQTT_Subscribe(&mqtt, "steyaertHome/masterBedroom/lightsStatus");
Adafruit_MQTT_Publish controlFeed = Adafruit_MQTT_Publish(&mqtt, "steyaertHome/masterBedroom/lightsControl");


// pins connected to your encoder (on Huzzah use any but 16 which is non-interrupt, 0 & 2 which have LEDs)
Encoder myEnc(13, 12);
long oldEncoderPosition  = -999;


// pinouts availbale 0 2 4 5 12 13 14 15 16
int timerTicks = 0;               // count ticks for interrupt timer

// create objects
#define BUTTON_COUNT 3
Button button[BUTTON_COUNT];
Utilities utilities;


// Time
// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const int timeZone = -5;     // Central Daylight Time
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
time_t prevDisplay = 0; // when the digital clock was displayed


void setup() {
  utilities.setup();
  
  statusFeed.setCallback(statusCallback);
  
  // Setup MQTT subscription for time feed.
  mqtt.subscribe(&statusFeed);
  MQTT_connect();

  // set up pushbuttons
  button[0].setup(0, 4, 2, buttonPress);
  button[1].setup(1, 5, 0, buttonPress);
  button[2].setup(2, 14, -1, buttonPress);


  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
}


void buttonPress(int id, bool value) {
  if (value) {
    if (id == 0) {
      MQTT_publish ("01:ON");
      MQTT_publish ("03:ON");
    } else if (id == 1) {
      MQTT_publish ("02:HUE:002");
      MQTT_publish ("02:SAT:100");
      MQTT_publish ("02:VAL:50");
      MQTT_publish ("02:ON");
    } else if (id == 2) {
      Serial.println("encoder push 1");
    }
    Serial.println("on");
  } else {
    if (id == 0) {
      MQTT_publish ("01:OFF");
      MQTT_publish ("03:OFF");
    } else if (id == 1) {
      MQTT_publish ("02:OFF");
    } else if (id == 2) {
      Serial.println("encoder push 2");
    }
    Serial.println("off");
  }
}


void loop() {
  // Ensure the connection to the MQTT server stays alive
  MQTT_connect();

  // check MQTT feed for updates
  mqtt.processPackets(20);

//  utilities.update();
  // updateInterrupts();     // allow interrupt timer to run

  // update pushbuttons
  for (int buttonID = 0; buttonID < BUTTON_COUNT; buttonID++) {
    button[buttonID].update();
  }

  long newPosition = myEnc.read();
  if (newPosition != oldEncoderPosition) {
    if (oldEncoderPosition < newPosition) {
      MQTT_publish ("02:BRIGHT");
    } else {
      MQTT_publish ("02:DIM");
    }
    oldEncoderPosition = newPosition;
//    String message = "02:VAL:";
//    int newValue = 64 + (newPosition/3);
//    if (newValue > 100) newValue = 100;
//    if (newValue < 0) newValue = 0;
//    message += newValue;
//    MQTT_publish (message);
//    Serial.println(newPosition);
  }
  
  updateInterrupts();     // allow interrupt timer to run
}

void updateInterrupts() {
  if (timerTicks % 2000 == 0) {
    utilities.loop(); // clock update
  } else if (timerTicks % 200 == 0) {
    // special case for double-button push
    if (digitalRead(4) == LOW && digitalRead(5) == LOW) {
      Serial.println("pushing both buttons");
    }
  }

  if (timerTicks++ > 2147483647) {
    timerTicks = 0;
  }
}


void statusCallback(char *data, uint16_t len) {
  Serial.print("Received: "); 
  Serial.println(data);
}


void MQTT_publish (String message) {
  // light status format: ID:{ON/OFF}:HUE:SAT:VAL
  char* charArray = "00:XXX:000:000:000";
  message.toCharArray(charArray, message.length()+1);
  // send out CONTROL message
  controlFeed.publish(charArray);
  Serial.print("Published:");
  Serial.println(message);
}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
     Serial.println(mqtt.connectErrorString(ret));
     Serial.println("Retrying MQTT connection in 5 seconds...");
     mqtt.disconnect();
     delay(5000);  // wait 5 seconds
     retries--;
     if (retries == 0) {
       // basically die and wait for WDT to reset me
       while (1);
     }
  }
  Serial.println("MQTT Connected!");
}


/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime() {
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
