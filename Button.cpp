#include "Button.h"

Button::Button() {
}

void Button::setup(int _buttonID, int _buttonPin, int _ledPin, GeneralBoolFunction _callback) {
  buttonID = _buttonID;
  ledPin = _ledPin;
  buttonPin = _buttonPin;
  callback = _callback;
  pinMode(buttonPin, INPUT);
  if (ledPin > -1) {
    pinMode(ledPin, OUTPUT);
    // set initial LED state
    digitalWrite(ledPin, ledState);
  }
}

void Button::update() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
//    Serial.println("toggle");
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        callback(buttonID, ledState == HIGH);
//        Serial.println("delay done and toggled");
      }
    }
  }

  if (ledPin > -1) {
    // set the LED:
    digitalWrite(ledPin, ledState);
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}
