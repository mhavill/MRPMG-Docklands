/*******************************
 * Header
 * Name: client.cpp
 * Purpose:  Establish a connection to a server and send data using HTTP POST method
 *  * Originally ESP32 code wascreated from esp32io.com and modified here
 * Created Date: 27/3/2025
 *******************************/

/*******************************
 * Includes
 *******************************/
#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <arduino-timer.h>

// NeoPixel
#include "neopixel.hpp"

/*******************************
 * Protptypes
 *******************************/

/*******************************
 * Definitions
 *******************************/
const char WIFI_SSID[] = "MRPMG";
const char WIFI_PASSWORD[] = "password";

String HOST_NAME = "http://esp32server.local";
String PATH_NAME = "/LEDupdate";
String queryString = "red=000&green=128&blue=128"; // DON'T exceed 255!!!
#define device "ESP32_client"
auto timer = timer_create_default(); // create a timer with default settings
const int SECOND = 1000;

/*******************************
 * Setup
 *******************************/

void setup()
{
  Serial.begin(115200);

  WiFi.hostname(device);
  delay(500);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  printf("Connecting to %s as %s\n", WIFI_SSID, device);

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32client"))
  {
    Serial.println("MDNS responder started");
  }

  HTTPClient http;

  http.begin(HOST_NAME + PATH_NAME);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(queryString);

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      // HTTP header has been sent and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    }
  }
  else
  {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  timer.every(0.5 * SECOND, npblink);
}
/*******************************
 * Loop
 *******************************/

void loop()
{
  timer.tick();
}
/*******************************
 * Utility Functions
 *******************************/
