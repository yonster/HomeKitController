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
  delay(200);

  // Clear the buffer.
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Good Morning!");

  display.setCursor(0,10);
  display.setTextSize(3);
  display.println("00:00");
  display.display();
}


//void Utilities::update() {
//}
//
//
//void Utilities::addMQTTObject(Object object) {
//  objects[objectCount++] = object;
////  object.setCallback(MQTT_publish);
////  bedroomLightsControlFeed.setCallback(object.tempCallback);
//  Serial.println("added");
//}


void Utilities::loop() {
//  if (timeStatus() != timeNotSet) {
//    if (now() != prevDisplay) { //update the display only if time has changed
//      prevDisplay = now();
      digitalClockDisplay();  
//    }
//  }
}

void Utilities::digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
//  printDigits(second());
//  Serial.print(" ");
//  Serial.print(day());
//  Serial.print(".");
//  Serial.print(month());
//  Serial.print(".");
//  Serial.print(year()); 
  Serial.println(); 

  // display on OLED screen
  display.clearDisplay();
  display.setCursor(0,10);
  display.setTextSize(3);
  display.print(hour());
  display.print(":");
  display.println(minute());
  display.display();
}



void Utilities::printDigits(int digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
