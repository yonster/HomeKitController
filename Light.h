/*
  Light.h - multi-threaded object code.
*/

#ifndef Light_h
#define Light_h
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

//typedef void (*GeneralMessageFunction) (char *message);
typedef void (*GeneralMessageFunction) (String message);
typedef void (*GeneralNeoPixelFunction) (int number, int red, int green, int blue);

class Light {
  public:
    Light();
    void setup(int startPixel, int endPixel, int lightID, char* lightIDString, GeneralNeoPixelFunction setPixelColor, GeneralMessageFunction callback);
    void updateValues();
    void processMessage(char *message);
    void toggle();
  private:
    int _startPixel = 0;
    int _endPixel = 0;
    int _lightID;
    int _hue = 0;
    int _saturation = 0;
    int _value = 0;
    char* _lightIDString = "00";
    char* _status = "ON";
    char* constructMessage();
    void getRGB(int hue, int sat, int val, int colors[3]);
    int _rgb_colors[3];
    GeneralNeoPixelFunction _setPixelColor;
    GeneralMessageFunction _callback;
};

#endif
