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
// int timerTicks = 0;               // count ticks for interrupt timer

// create objects
#define BUTTON_COUNT 3
Button button[BUTTON_COUNT];
Utilities utilities;


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
}


void buttonPress(int id, bool value) {
  // light status format: ID:{ON/OFF}:HUE:SAT:VAL
  if (value) {
    if (id == 0) {
      MQTT_publish ("01:ON");
      MQTT_publish ("03:ON");
//      MQTT_publish ("01:ON:050:023:050");
//      MQTT_publish ("03:ON:050:023:050");
    } else if (id == 1) {
      MQTT_publish ("02:ON");
      MQTT_publish ("02:HUE:002");
      MQTT_publish ("02:SAT:100");
      MQTT_publish ("02:VAL:50");
//      MQTT_publish ("02:ON:002:255:255");
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
}


void statusCallback(char *data, uint16_t len) {
  Serial.print("Received: "); 
  Serial.println(data);
}


/* if we need to add multi-threading later...
void updateInterrupts() {
  if (timerTicks++ > 2001) {
    timerTicks = 0;
    timerTicks = 0;
  } else if (timerTicks % 100 == 0) {
    light[0].update();
    light[1].update();
  }
}
*/


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
