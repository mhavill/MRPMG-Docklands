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
#include <arduino-timer.h>

// #include "secrets.h"

// Include the libraries we need for temperature
#include <OneWire.h>
#include <DallasTemperature.h>
// Webpage
#include "html.h"
#include <iostream>
#include <string>

// NeoPixel
#include "neopixel.hpp"

/*******************************
 * Protptypes
 *******************************/

void handleNotFound();
void setup(void);
bool readtemp(void *);
void MainPage();
void Temp();
void LEDControl();

/*******************************
 * Definitions
 *******************************/

const char *ssid = "MRPMG";
const char *password = "password";
#define device "ESP32_server"
const int SECOND = 1000;
static float tempC;
// static DeviceAddress deviceAddress;
// static bool waitForConversion = false;

// DONE Add Code to read temperature
// TODO Add Code to manage LED(s)
// DONE handle client GET
// TODO handle client PUT/POST

WebServer server(80);

const int LED = 2;
// DS1820 Data wire is plugged into GPIO 10 on the ESP32
#define ONE_WIRE_BUS 10

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

auto timer = timer_create_default(); // create a timer with default settings

/*******************************
 * Setup
 *******************************/
void setup(void)
{
  // Start the neopixel
  npsetup();
  // Start the serial port
  Serial.begin(115200);
  delay(SECOND);
  // Start the WiFi
  // WiFi.mode(WIFI_STA);
  WiFi.hostname(device);
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
  // Start up the temperature library
  sensors.begin();

  timer.every(2 * SECOND, readtemp);
  timer.every(0.5 * SECOND, npblink);

  server.on("/", MainPage); /*Client request handling: calls the function to serve HTML page */

  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });

  server.on("/readTemp", Temp);

  server.on("/LEDupdate", LEDControl);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  sensors.setWaitForConversion(false);
}

/*******************************
 * Loop
 *******************************/
// DONE Remove blocking code
void loop(void)
{
  server.handleClient();
  // delay(2); // allow the cpu to switch to other tasks
  timer.tick();
  // nploop();
}

/*******************************
 * Utility Functions
 *******************************/

// DONE Investigate if this is the best way to read the temperature as this appears to block the loop code - set waitForConversion to false

bool readtemp(void *)
{
  Serial.print("Requesting temperature...");
  sensors.requestTemperaturesByIndex(0); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  return true;
}

// DONE Change to Neopixel

void handleNotFound()
{

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
  Serial.println(message);
}
void MainPage()
{
  String _html_page = html_page;             /*Read The HTML Page*/
  server.send(200, "text/html", _html_page); /*Send the code to the web server*/
}

void Temp()
{
  String TempValue = String(tempC);          // Convert it into string
  server.send(200, "text/plane", TempValue); // Send updated temperature value to the web server
}

void LEDControl()
{
  // Stop the LED from blinking
  blink = false;
  Serial.println("LED Control activated");
  String message = "LED Control activated\n\n";
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
  Serial.print(message);
  uint8_t LEDred = server.arg(0).toInt(); // Convert and read the LED red
  uint8_t LEDgreen = server.arg(1).toInt();  // Convert and read the LED green
  uint8_t LEDblue = server.arg(2).toInt();   // Convert and read the LED blue
  colorWipe(strip.Color(LEDred, LEDgreen, LEDblue), 50); // Set the LED colour

  server.send(200, "text/plane", message); // Send the LED status to the web server
}
