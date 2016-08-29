/*
  Utilities.h - setup utilities
*/

#ifndef Utilities_h
#define Utilities_h
#include "Arduino.h"
#include "Object.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "ATTuNvpygs"
#define WLAN_PASS       "f4hee83p6qir"
#include <ESP8266WiFi.h>

/************************* MQTT Setup *********************************/

#define MQTT_SERVER      "192.168.1.65"
#define MQTT_PORT   1883        // use 8883 for SSL
#define MQTT_USERNAME    ""
#define MQTT_PASSWORD    ""
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif


class Utilities {
  public:
    Utilities();
    void setup();
//    void MQTT_connect();
//    void MQTT_publish(String message);
//    void update();
//    void addMQTTObject(Object object);
  private:
};

#endif
