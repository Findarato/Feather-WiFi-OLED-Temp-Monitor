// Now using ESP8266.....
// Sample Arduino Json Web Server
// Created by Benoit Blanchon.
// Heavily inspired by "Web Server" from David A. Mellis and Tom Igoe
// http://www.esp8266.com/viewtopic.php?f=29&t=7158

#include <SPI.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

//////////////////////////////
// DHT21 / AMS2301 is at GPIO2
//////////////////////////////
#define DHTPIN 14

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11   // DHT 11 
// #define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT21   // DHT 21 (AM2301)

// init DHT; 3rd parameter = 16 works for ESP8266@80MHz
DHT dht(DHTPIN, DHTTYPE,12); 

#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);
 
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
 
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// this constant won't change:
const int  buttonPinIP = 0;    // the pin that the pushbutton is attached to
const int  buttonPinTemp = 16;    // the pin that the pushbutton is attached to
const char* ssid = "SSID";
const char* password = "Password";
const char* DeviceName = "Location";

//Button A is #0
//Button B is #16
//Button C is #2


// Variables will change:
int buttonPushCounterIP = 0;   // counter for the number of button presses
int buttonStateIP = LOW;         // current state of the button
int lastButtonStateIP = HIGH;     // previous state of the button


int buttonPushCounterTemp = 0;   // counter for the number of button presses
int buttonStateTemp = LOW;         // current state of the button
int lastButtonStateTemp = HIGH;     // previous state of the button

// needed to avoid link error on ram check
extern "C" {
#include "user_interface.h"
uint16 readvdd33(void);
}
ADC_MODE(ADC_VCC);

WiFiServer server(80);


float pfDew,pfHum,pfTemp,pfVcc,pfTempF,battery;


void connectionInfo () {
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(ssid);
  display.println(WiFi.localIP());
  display.println("Device Location");
  display.print(DeviceName);
  display.display();
}


void readTempValues() {
   pfTemp = dht.readTemperature();   
   pfHum = dht.readHumidity();
   pfTempF = dht.readTemperature(true);
   float a = 17.67;
   float b = 243.5;
   float alpha = (a * pfTemp)/(b + pfTemp) + log(pfHum/100);
   pfDew = (b * alpha)/(a - alpha);


  Serial.println(pfTemp);
  Serial.println(pfTempF);
  Serial.println(pfHum);   
  Serial.println(pfDew);


  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Temp C: ");
  display.println(pfTemp);
  display.print("Temp F: ");
  display.println(pfTempF);
  display.print("Humidity: ");
  display.println(pfHum);
  display.print("Duepoint: ");
  display.println(pfDew);
  display.display();

  
//  Serial.println(pfTemp);
//  Serial.println(pfTemp);
}

bool readRequest(WiFiClient& client) {
  bool currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n' && currentLineIsBlank) {
        return true;
      } else if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }
  return false;
}

JsonObject& prepareResponse(JsonBuffer& jsonBuffer) {
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& InfoValues = root.createNestedArray("Info");
    InfoValues.add("1");
    InfoValues.add(DeviceName);
  JsonArray& tempValues = root.createNestedArray("temperature");
    tempValues.add(pfTemp);
    tempValues.add(pfTempF);
  JsonArray& humiValues = root.createNestedArray("humidity");
    humiValues.add(pfHum);
  JsonArray& dewpValues = root.createNestedArray("dewpoint");
    dewpValues.add(pfDew);
  JsonArray& EsPvValues = root.createNestedArray("Systemv");
    EsPvValues.add(pfVcc/1000,3);  
    EsPvValues.add(battery/1000,3);
  return root;
}

void writeResponse(WiFiClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  json.prettyPrintTo(client);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Up!");
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(100);

    // Connect to WiFi network
  WiFi.begin(ssid, password);  

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  delay(200);
 
  // Clear the buffer.
  display.clearDisplay();
  display.println("Startup Temp IoT Thingy");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.println("");
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Connecting to: ...");
    display.println(ssid);
    display.display();
  }

  
  dht.begin();
 
  connectionInfo();
  delay(2000);

  // Start the server
  server.begin();
  Serial.println("Server started");

// initialize the button pin as a input:
  pinMode(buttonPinIP, INPUT_PULLUP);
  pinMode(buttonPinTemp, INPUT_PULLUP);
  
}

void loop() {
  
  WiFiClient client = server.available();
  if (client) {
      bool success = readRequest(client);
      if (success) {
          delay(1000);
          readTempValues();
          StaticJsonBuffer<500> jsonBuffer;
          JsonObject& json = prepareResponse(jsonBuffer);
          writeResponse(client, json);
      }
      delay(1);
      client.stop();
  }
  



  // read the pushbutton input pin:
  buttonStateIP = digitalRead(buttonPinIP);
   // compare the buttonState to its previous state
  if (buttonStateIP != lastButtonStateIP) {
    // if the state has changed, increment the counter
    connectionInfo();
  } 

  buttonStateTemp = digitalRead(buttonPinTemp);
   // compare the buttonState to its previous state
  if (buttonStateTemp != lastButtonStateTemp) {
    // if the state has changed, increment the counter
    readTempValues();
  } 


}