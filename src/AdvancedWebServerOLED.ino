#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include "Pubvars.h"

#include <ArduinoJson.h>

// Defines
#define DHTPIN 14
#define DHTTYPE DHT21 // DHT 21 (AM2301)



DHT dht(DHTPIN, DHTTYPE, 12);            // this constant won't change:
int buttonStateIP = LOW;                 // current state of the button
int lastButtonStateIP = HIGH;            // previous state of the button
int buttonStateTemp = LOW;               // current state of the button
int lastButtonStateTemp = HIGH;          // previous state of the button
int buttonStateToggleDisplay = LOW;      // current state of the button
int lastButtonStateToggleDisplay = HIGH; // last state of the botton
bool toggleDisplay = true;               // Turns the display on or off
int tracker = 0;                         // How many times it has tried to connect
byte mac[6];                             // the MAC address of your Wifi shield
String hardwareID;                       // Display of the mac address
Varstore vault;
int lastArea = 9;

WiFiServer server(vault.readServerPort());

float pfDew, pfHum, pfTemp, pfVcc, pfTempF, battery; // Setting up some variable states

String macToStr(const uint8_t *mac)
{
    String result;
    for (int i = 0; i < 6; ++i)
    {
        result += String(mac[i], 16);
        if (i < 5)
            result += ':';
    }
    return result;
}

void connectionInfo()
{
    if (lastArea != 0)
    {
        WiFi.macAddress(mac);
        hardwareID = macToStr(mac);
    }
}
/**
 * Read Temperature data from DHT sensor
 * @method readTempValues
 */
void readTempValues()
{
    pfTemp = dht.readTemperature();
    pfHum = dht.readHumidity();
    pfTempF = dht.readTemperature(true);
    float a = 17.67;
    float b = 243.5;
    float alpha = (a * pfTemp) / (b + pfTemp) + log(pfHum / 100);
    pfDew = (b * alpha) / (a - alpha);

    pfDew = pfTemp - ((100 - pfHum) / 5);

    Serial.println(pfTemp);
    Serial.println(pfTempF);
    Serial.println(pfHum);
    Serial.println(pfDew);
}
bool readRequest(WiFiClient &client)
{
    bool currentLineIsBlank = true;
    while (client.connected())
    {
        if (client.available())
        {
            char c = client.read();
            if (c == '\n' && currentLineIsBlank)
            {
                return true;
            }
            else if (c == '\n')
            {
                currentLineIsBlank = true;
            }
            else if (c != '\r')
            {
                currentLineIsBlank = false;
            }
        }
    }
    return false;
}

JsonDocument prepareResponse()
{
    long rssi = WiFi.RSSI();
    Serial.print("RSSI:");
    Serial.println(rssi);
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["rssi"] = rssi;
    jsonDoc["DeviceName"] = vault.readDeviceName();
    jsonDoc["DeviceID"] = vault.readDeviceID();
    jsonDoc["HardwareID"] = hardwareID;
    jsonDoc["tempF"] = pfTempF;
    jsonDoc["tempC"] = pfTemp;
    jsonDoc["humidity"] = pfHum;
    jsonDoc["dewpoint"] = pfDew;
    return jsonDoc;
}

void writeResponse(WiFiClient &client, JsonDocument json)
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println(measureJsonPretty(json));
    client.println();
    serializeJsonPretty(json, client);
}

void setup()
{
    Serial.begin(115200);
    Serial.print("Connecting to ");
    Serial.println(vault.readSSID());
    delay(100);
    // Connect to WiFi network
    WiFi.begin(vault.readSSID(), vault.readPassword());
    WiFi.softAPdisconnect();
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    delay(200);

    // Clear the buffer.
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(2000);
        Serial.print("");
        Serial.println("");
        Serial.print("Connecting to: ");
        Serial.print(vault.readSSID());
        Serial.println("");
        // Serial.print("Password: ");
        // Serial.print(vault.readPassword());
        // Serial.println("");
        Serial.print("Status: ");
        Serial.print(WiFi.status());
        Serial.println("");
        Serial.print("IP Address: ");
        Serial.print(WiFi.localIP());
        Serial.println("");
        Serial.print("Loop: ");
        Serial.print(tracker);
        Serial.println("");

        tracker++;
    }

    dht.begin();

    connectionInfo();
    delay(5000);

    // Start the server
    server.begin();
    Serial.println("IP Address: ");
    Serial.print(WiFi.localIP());
    Serial.println("Server started");

}

void loop()
{
    WiFiClient client = server.available();
    if (client)
    { // We have a client connected
        bool success = readRequest(client);
        if (success)
        {
            delay(1000); // Wait a second to ensure that we do not overload the seonsor
            readTempValues();
            JsonDocument json = prepareResponse();
            writeResponse(client, json);
        }
        delay(10); // pause 10 milliseconds and then kill the connection
        client.stop();
    }

}
