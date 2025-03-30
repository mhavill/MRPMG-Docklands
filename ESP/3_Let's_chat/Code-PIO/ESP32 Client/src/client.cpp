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
void setup(void);
void loop(void);
bool readJoystick(void *argument);
void postHTTP();

/*******************************
 * Definitions
 *******************************/
const char WIFI_SSID[] = "MRPMG";
const char WIFI_PASSWORD[] = "password";

String HOST_NAME = "http://esp32server.local";
String PATH_NAME = "/LEDupdate";
String queryString = "red=000&green=128&blue=128"; // DON'T exceed 255!!!
#define device "ESP32client"
auto timer = timer_create_default(); // create a timr with default settings
const int SECOND = 1000;

// Joystick
#define JOY_X 35
#define JOY_Y 34
#define JOY_BUTTON 39
bool oldButtonState = HIGH;

// LED variables
uint8_t RED = 0;
uint8_t GREEN = 0;
uint8_t BLUE = 0;

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

  if (MDNS.begin(device))
  {
    Serial.println("MDNS responder started");
  }
  postHTTP();
  // setup the Joystick
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(JOY_BUTTON, INPUT_PULLUP);

  timer.every(0.5 * SECOND, npblink);
  timer.every(0.5 * SECOND, readJoystick);
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

bool readJoystick(void *)
{
  int x = analogRead(JOY_X);
  int y = analogRead(JOY_Y);
  bool button = digitalRead(JOY_BUTTON);

  Serial.printf("X: %d, Y: %d, Button: %d oldButtonState: %d\n", x, y, button, oldButtonState);
  RED = map(x, 0, 4095, 0, 127);
  GREEN = map(y, 0, 4095, 0, 127);
  BLUE = button == LOW ? 0 : 127;

  colorWipe(strip.Color(RED, GREEN, BLUE), 50);
  if (JOY_BUTTON != oldButtonState)
  {
    oldButtonState = JOY_BUTTON;
    Serial.print("Button Pressed: ");
    queryString = "red=";
    queryString += RED;
    queryString += "&green=";
    queryString += GREEN;
    queryString += "&blue=";
    queryString += BLUE;
    Serial.println(queryString);
    postHTTP();
  }

  return true;
}

void postHTTP()
{
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
}