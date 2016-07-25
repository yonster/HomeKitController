#include "Light.h"

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
#include <Adafruit_NeoPixel.h>
#define NEOPIXEL_PIN   0
#define NUMPIXELS      240
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define LIGHT_COUNT 3

// const int sensorPin = 1;  // pin the potmeter is attached too
// int sensorVal = 0; // store the value coming from the sensor

#define MQTT_SERVER      "192.168.1.65"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "ATTuNvpygs"
#define WLAN_PASS       "f4hee83p6qir"
//#define WLAN_SSID       "argonet2"
//#define WLAN_PASS       "3atth3nautilu5"

/****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* MQTT Setup *********************************/

#define MQTT_PORT   1883        // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_PASSWORD    ""

/************************ Global Variables & States **************************************/

int timerTicks = 0;               // count ticks for interrupt timer

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Light Characteristics: On/Brightness/Hue/Saturation/Name

// Setup feeds for sub/publishing.
Adafruit_MQTT_Publish bedroomLightsStatusFeed = Adafruit_MQTT_Publish(&mqtt, "steyaertHome/masterBedroom/lightsStatus");
// can also add final parameter: MQTT_QOS_1 to turn on QOS

Adafruit_MQTT_Subscribe bedroomLightsControlFeed = Adafruit_MQTT_Subscribe(&mqtt, "steyaertHome/masterBedroom/lightsControl");

// Subscribe to these from Terminal with e.g.:
// mosquitto_sub -h 192.168.1.65 -d -t steyaertHome/masterBedroom/lightsStatus


/*************************** Sketch Code ************************************/

// create objects
Light light[LIGHT_COUNT];

#ifdef __AVR__
  #include <avr/power.h>
#endif

void setup() {
  Serial.begin(115200);
  delay(10);

  // set up lights
  light[0].setup( 0, 19, 0, "01", setPixelColor, MQTT_publish);
  light[1].setup(20, 39, 1, "02", setPixelColor, MQTT_publish);
  light[2].setup(40, 59, 2, "03", setPixelColor, MQTT_publish);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for receiving control feed.
  mqtt.subscribe(&bedroomLightsControlFeed);

  pixels.begin();  // This initializes the NeoPixel library.
  pixels.show();   // Initialize all pixels to 'off'
}


void setPixelColor (int number, int red, int green, int blue) {
  pixels.setPixelColor(number, pixels.Color(red, green, blue));
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

void loop() {
  // Ensure the connection to the MQTT server stays alive
  MQTT_connect();
  // updateInterrupts();     // allow interrupt timer to run

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;

  while ((subscription = mqtt.readSubscription())) {
    char* message;
    
      // Process based on update topic.
    if (subscription == &bedroomLightsControlFeed) {
      message = (char*)bedroomLightsControlFeed.lastread;
      Serial.print("Received: ");
      Serial.println(message);
      for (int lightID = 0; lightID < LIGHT_COUNT; lightID++) {
        light[lightID].processMessage(message);
      }
    } else {
      Serial.println("Message received on unknown topic");
      break;
    }
  }
}

/*
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
  char* charArray = "00:XXX:000";
  message.toCharArray(charArray, message.length()+1);
  bedroomLightsStatusFeed.publish(charArray);
  Serial.print("Published:");
  Serial.println(message);
  pixels.show();
}
