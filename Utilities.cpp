/*************************   Various Setup   *********************************/
#include "Utilities.h"

// Display
#define OLED_RESET 16
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


Utilities::Utilities() {
}

void Utilities::setup() {
  // Subscribe to these from Terminal with e.g.:
  // mosquitto_sub -h 192.168.1.65 -d -t steyaertHome/masterBedroom/lightsStatus

  Serial.begin(115200);
  delay(5);
  
  // Connect to WiFi access point.
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

  // Display setup: by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(100);

  // Clear the buffer.
  display.clearDisplay();
}


void Utilities::loop() {
  updateDisplay();
}


void Utilities::updateMode(int increment) {
  // we need a sensitivity reducer
  encoderAccumulator += increment;
   if (encoderAccumulator > ENCODER_DAMPENING) {
    currentModeID += 1;
    if (currentModeID + 1 > MODEARRAYSIZE) {
      currentModeID = 0;
    }
    encoderAccumulator = 0;
    updateDisplay();
  } else if (encoderAccumulator < -ENCODER_DAMPENING) {
    currentModeID -= 1;
    if (currentModeID < 0 ) {
      currentModeID = MODEARRAYSIZE - 1;
    }
    encoderAccumulator = 0;
    updateDisplay();
  }
}


void Utilities::updateDisplay() {
  // update all display items on OLED screen
  display.clearDisplay();
  if (currentModeID == 1) {
    // we're in DARK mode
  } else {
    // show time/date
    digitalClockDisplay();
  }
  if (modeVisible) {
    displayString(modeArray[currentModeID], 72, 24, 1);
  }
  display.display();
}

void Utilities::displayString(String theString, int x = 0, int y = 0, int fontSize = 1) {
    // display on OLED screen
  Serial.println(theString);
  display.setCursor(x,y);
  display.setTextColor(WHITE);
  display.setTextSize(fontSize);
  display.print(theString);
}


void Utilities::digitalClockDisplay() {
  displayString(hour() + addZero(minute()), 0, 0, 2);
  displayString(year() + addDot(month()) + addDot(day()), 0, 24, 1);
}


String Utilities::addDot(int digits) {
  return "." + String(digits);
}


String Utilities::addZero(int digits) {
  // utility for digital clock display: retruns preceding colon and leading 0
  String result = ":";
  if(digits < 10)
    result += '0';
  return result + digits;
}
