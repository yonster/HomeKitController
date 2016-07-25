#include "Light.h"

/*
  Light.cpp - drives Neopixel gravity object
  Created by Ian Steyaert, Dec 13, 2015.
  Released into the public domain.

  Control a RGB led with Hue, Saturation and Brightness (HSB / HSV )

  Hue is change by an analog input.
  Brightness is changed by a fading function.
  Saturation stays constant at 255

  getRGB() function based on <http://www.codeproject.com/miscctrl/CPicker.asp>
  dim_curve idea by Jims <http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1174674545>

  created 05-01-2010 by kasperkamperman.com
*/

/*
  dim_curve 'lookup table' to compensate for the nonlinearity of human vision.
  Used in the getRGB function on saturation and brightness to make 'dimming' look more natural.
  Exponential function used to create values below :
  x from 0 - 255 : y = round(pow( 2.0, x+64/40.0) - 1)
*/

const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

Light::Light() {
}

void Light::setup(int startPixel, int endPixel, int lightID, char* lightIDString, GeneralNeoPixelFunction setPixelColor, GeneralMessageFunction callback) {
  _startPixel = startPixel;
  _endPixel = endPixel;
  _lightID = lightID;
  _setPixelColor = setPixelColor;
  _callback = callback;
  _lightIDString= lightIDString;
}

void Light::updateValues() {
  getRGB(_hue, _saturation, _value, _rgb_colors);   // converts HSB to RGB

  if (_status == "ON") {
    for(int i=_startPixel;i<=_endPixel;i++) {
      _setPixelColor(i, _rgb_colors[0], _rgb_colors[1], _rgb_colors[2]);
    }
  } else {
    for(int i=_startPixel;i<=_endPixel;i++) {
      _setPixelColor(i, 0, 0, 0);
    }
  }

  // set updated status
  _callback(String(_lightIDString) + ":ON:" + String(_status));
  _callback(String(_lightIDString) + ":VAL:" + String(_value));
  _callback(String(_lightIDString) + ":HUE:" + String(_hue));
  _callback(String(_lightIDString) + ":SAT:" + String(_saturation));
}

void Light::processMessage(char *message) {
  // messages will come in as:
  // ID: FXN: VAL
  // Light ID (00-99): Function (ON|OFF|HUE|SAT|VAL): Value (0-1000)
  // eg. 01: HUE: 320

  if (strncmp(message, _lightIDString, 2) == 0) {   // compare first 2 chars to lightID
    // if matches our ID, process request
    String messageString = String(message);

    if (messageString.substring(3,5) == "ON") {
      _status = "ON";
    } else if (messageString.substring(3,6) == "OFF") {
      _status = "OFF";
    } else if (messageString.substring(3,9) == "CHECKSTATUS") {
//      light("ON_UPDATE");
    } else if (messageString.substring(3,11) == "IDENTIFY") {
      for (int j=0; j<4; j++) {
        for(int i=_startPixel;i<=_endPixel;i++) {
          _setPixelColor(i, 0, 255, 0);
        }
        delay(500);
        for(int i=_startPixel;i<=_endPixel;i++) {
          _setPixelColor(i, 0, 0, 0);
        }
        delay(500);
      }
    } else {
      int value = atoi(&message[7]);
      if (messageString.substring(3,6) == "HUE") {
        _hue = value;
      } else if (messageString.substring(3,6) == "SAT") {
        _saturation = value * 255/100;
      } else if (messageString.substring(3,6) == "VAL") {
        _value = value * 255/100;
      }
    }
  }
  updateValues();
}

void Light::toggle() {
  
}


void Light::getRGB(int hue, int sat, int val, int colors[3]) {
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.
  */

  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];

  int r;
  int g;
  int b;
  int base;

  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;
  } else  {

    base = ((255 - sat) * val)>>8;

    switch(hue/60) {
  case 0:
    r = val;
    g = (((val-base)*hue)/60)+base;
    b = base;
  break;

  case 1:
    r = (((val-base)*(60-(hue%60)))/60)+base;
    g = val;
    b = base;
  break;

  case 2:
    r = base;
    g = val;
    b = (((val-base)*(hue%60))/60)+base;
  break;

  case 3:
    r = base;
    g = (((val-base)*(60-(hue%60)))/60)+base;
    b = val;
  break;

  case 4:
    r = (((val-base)*(hue%60))/60)+base;
    g = base;
    b = val;
  break;

  case 5:
    r = val;
    g = base;
    b = (((val-base)*(60-(hue%60)))/60)+base;
  break;
    }

    colors[0]=r;
    colors[1]=g;
    colors[2]=b;
  }
}
