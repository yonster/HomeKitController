/*
  Utilities.h - setup utilities
*/

#ifndef Utilities_h
#define Utilities_h
#include "Arduino.h"
#include "Object.h"

// Time
#include <SPI.h>
#include <Wire.h>
#include <TimeLib.h> 
#include <WiFiUdp.h>

// Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8x8lib.h>
/* Constructor */
//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 13, /* data=*/ 11);

#define MODEARRAYSIZE 5
#define MODEARAY      "NORMAL", "DARK", "RAINBOW", "WAKE", "CLOCK", "SUNRISE"

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

#define ENCODER_DAMPENING 3

class Utilities {
  public:
    Utilities();
    void setup();
    void loop();
    void displayString(String theString, int x, int y, int fontSize);
    void updateDisplay();
    void updateMode(int increment);
    void digitalClockDisplay();
    String addZero(int digits);
    String addDot(int digits);
    bool modeVisible = false;
    String modeArray[MODEARRAYSIZE] = {MODEARAY};
    int currentModeID = 0;
    int encoderAccumulator = 0;
  private:
};

#endif
