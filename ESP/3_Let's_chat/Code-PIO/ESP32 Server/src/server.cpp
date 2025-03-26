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

/*******************************
 * Protptypes
 *******************************/

void handleNotFound();
void setup(void);
bool readtemp(void *);
void MainPage();
void Temp();

/*******************************
 * Definitions
 *******************************/

const char *ssid = "MRPMG";
const char *password = "password";
const int SECOND = 1000;
static float tempC;

// TODO Add Code to read temperature
// TODO Add Code to manage LED(s)
// TODO handle client GET
// TODO handle client PUT

WebServer server(80);

const int led = 2;
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
  // Start up the temperature library
  sensors.begin();
  timer.every(2 * SECOND, readtemp);

  server.on("/", MainPage); /*Client request handling: calls the function to serve HTML page */

  server.on("/inline", []()
            { server.send(200, "text/plain", "this works as well"); });

  server.on("/readTemp", Temp);

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
  // delay(2); // allow the cpu to switch to other tasks
  timer.tick();
}

/*******************************
 * Utility Functions
 *******************************/
bool readtemp(void *)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
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

// TODO Change to Neopixel

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
