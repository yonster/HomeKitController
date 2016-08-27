/*
  Button.h - multi-threaded object code.
  Based on http://www.arduino.cc/en/Tutorial/Debounce
*/

#ifndef Button_h
#define Button_h
#include "Arduino.h"
#include "Object.h"

class Button {
  public:
    Button();
    void setup(int _buttonID, int _buttonPin, int _ledPin, GeneralBoolFunction _callback);
    void update();
  private:
    int buttonID = 0;     // id number
    int buttonPin = 0;    // the number of the pushbutton pin
    int ledPin = 0;       // the number of the LED pin, -1 for none
    int ledState = HIGH;          // the current state of the output pin
    
    // Variables will change:
    int buttonState = HIGH;       // the current reading from the input pin
    int lastButtonState = LOW;    // the previous reading from the input pin
    
    // the following variables are long's because the time, measured in miliseconds,
    // will quickly become a bigger number than can be stored in an int.
    long lastDebounceTime = 0;        // the last time the output pin was toggled
    const long debounceDelay = 50;    // the debounce time; increase if the output flickers

    GeneralBoolFunction callback;
};

#endif
