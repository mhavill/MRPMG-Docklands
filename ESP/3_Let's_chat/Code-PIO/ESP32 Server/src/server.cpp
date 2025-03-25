/*******************************
 * Header
 * Name: server.cpp
 * Purpose: Sample Webserver
 * Created Date: 26/03/2025
 *******************************/

/*******************************
 * Includes
 *******************************/
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "secrets.h"

// Include the libraries we need for temperature
#include <OneWire.h>
#include <DallasTemperature.h>

/*******************************
 * Protptypes
 *******************************/
void handleRoot();
void handleNotFound();
void setup(void);

/*******************************
 * Definitions
 *******************************/

// const char* ssid = "MRPMG";
// const char* password = "password";

// TODO Add Code to read temperature
// TODO Add Code to manage LED(s)

WebServer server(80);

const int led = 2;
// DS1820 Data wire is plugged into GPIO 10 on the ESP32
#define ONE_WIRE_BUS 10

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

/*******************************
 * Setup
 *******************************/
void setup(void)
{
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

/*******************************
 * Loop
 *******************************/
// TODO Remove blocking code
void loop(void)
{
  server.handleClient();
  delay(2); // allow the cpu to switch to other tasks
}

/*******************************
 * Utility Functions
 *******************************/

// TODO Change to Neopixel
void handleRoot()
{
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}

void handleNotFound()
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
