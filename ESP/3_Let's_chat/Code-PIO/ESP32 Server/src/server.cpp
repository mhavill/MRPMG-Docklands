/*******************************
 * Header
 * Name: server.cpp
 * Purpose: Sample Webserver woth 64x32 RGB display
 * Created Date: 26/03/2025
 * Updated Date: 15/1/2026
 *******************************/

/*******************************
 * Includes
 *******************************/
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <arduino-timer.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include "web_server.h"
#include "audio_reactive.h"

// #include "secrets.h"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <GFX_Layer.hpp>

// Webpage
#include "html.h"
#include <iostream>
#include <string>

// NeoPixel
#include "neopixel.hpp"

/*******************************
 * Protptypes
 *******************************/

void handleNotFound(AsyncWebServerRequest *request);
void setup();
void loop();
String urlDecode(String input);
void MainPage(AsyncWebServerRequest *request);
void MainPageSubmit(AsyncWebServerRequest *request);
void LEDControl(AsyncWebServerRequest *request);
void layer_draw_callback(int16_t x, int16_t y, uint8_t r_data, uint8_t g_data, uint8_t b_data);
uint16_t colorWheel(uint8_t pos);
void printTextRainbowCentered(int colorWheelOffset, const char *text, int yPos);
void scrollText(int colorWheelOffset, const char *text);
void drawTextCentered(int colorWheelOffset, const char *text, int yPos);
void updateBackground();
void text_display();
void vumeter_display();
bool display(void *);
bool wmloop(void *);

/*******************************
 * Definitions
 *******************************/

#define device "ESP32server"
const int SECOND = 1000;
static float tempC;

// // AsyncWebServer server(80);

// const int LED = 2;
// #define EEPROM_SIZE 5
// #define LED_PIN 2
// #define M_WIDTH 16
// #define M_HEIGHT 16
// #define NUM_LEDS (M_WIDTH * M_HEIGHT)

// #define EEPROM_BRIGHTNESS 0
// #define EEPROM_GAIN 1
// #define EEPROM_SQUELCH 2
// #define EEPROM_PATTERN 3
// #define EEPROM_DISPLAY_TIME 4

// uint8_t numBands;
// uint8_t barWidth;
// uint8_t pattern;
// uint8_t brightness;
// uint16_t displayTime;
// bool autoChangePatterns = false;

// uint8_t peak[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// uint8_t prevFFTValue[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// uint8_t barHeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// // Colors and palettes
// DEFINE_GRADIENT_PALETTE(purple_gp){
//     0, 0, 212, 255,    // blue
//     255, 179, 0, 255}; // purple
// DEFINE_GRADIENT_PALETTE(outrun_gp){
//     0, 141, 0, 100,   // purple
//     127, 255, 192, 0, // yellow
//     255, 0, 5, 255};  // blue
// DEFINE_GRADIENT_PALETTE(greenblue_gp){
//     0, 0, 255, 60,    // green
//     64, 0, 236, 255,  // cyan
//     128, 0, 5, 255,   // blue
//     192, 0, 236, 255, // cyan
//     255, 0, 255, 60}; // green
// DEFINE_GRADIENT_PALETTE(redyellow_gp){
//     0, 200, 200, 200,    // white
//     64, 255, 218, 0,     // yellow
//     128, 231, 0, 0,      // red
//     192, 255, 218, 0,    // yellow
//     255, 200, 200, 200}; // white
// CRGBPalette16 purplePal = purple_gp;
// CRGBPalette16 outrunPal = outrun_gp;
// CRGBPalette16 greenbluePal = greenblue_gp;
// CRGBPalette16 heatPal = redyellow_gp;
// uint8_t colorTimer = 0;

auto timer = timer_create_default(); // create a timer with default settings

/*  Default library pin configuration for the reference
  you can redefine only ones you need later on object creation
  */
#define R1 25
#define G1 26
#define BL1 27
#define R2 14
#define G2 12
#define BL2 13
#define CH_A 23
#define CH_B 22
#define CH_C 5
#define CH_D 18 // replaced from pin 17 is faulty?
#define CH_E 32 // assign to any available pin if using panels with 1/32 scan
#define CLK 19  // replaced from pin 16 is faulty?
#define LAT 4
#define OE 15

// Configure for your panel(s) as appropriate!
#define PIN_E 32
#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32 // Panel height of 64 will required PIN_E to be defined.

#define PANELS_NUMBER 1 // Number of chained panels, if just a single panel, obviously set to 1

#define PANE_WIDTH PANEL_WIDTH *PANELS_NUMBER
#define PANE_HEIGHT PANEL_HEIGHT
// #define NUM_LEDS PANE_WIDTH *PANE_HEIGHT   

uint16_t colorWheel(uint8_t pos);
char ssid[32] = {0};

//------------------------------------------------------------------------------------------------------------------

MatrixPanel_I2S_DMA *dma_display = nullptr;

//------------------------------------------------------------------------------------------------------------------

//====================== Variables For scrolling Text=====================================================
unsigned long isAnimationDue;
int delayBetweeenAnimations = 18;               // Smaller == faster
int textXPosition = PANEL_WIDTH * CHAIN_LENGTH; // Will start off screen
int textYPosition = PANEL_HEIGHT / 2 - 7;       // center of screen - 8 (half of the text height)
//====================== Variables For scrolling Text=====================================================

// Pointers to this variable will be passed into getTextBounds,
// they will be updated from inside the method
int16_t xOne, yOne;
uint16_t w, h;

unsigned long last_increment = 0;
unsigned long increment_amt = 0;
// Global GFX_Layer object
GFX_Layer gfx_layer_bg(PANEL_WIDTH *CHAIN_LENGTH, PANEL_HEIGHT, layer_draw_callback); // background
GFX_Layer gfx_layer_fg(PANEL_WIDTH *CHAIN_LENGTH, PANEL_HEIGHT, layer_draw_callback); // foreground

GFX_LayerCompositor gfx_compositor(layer_draw_callback);

uint8_t wheelval = 0;

std::string upper_msg;
std::string lower_msg;
std::string top_msg;
uint16_t background = 10;
uint16_t speed = 18;

enum display_type
{
  DISPLAY_TEXT,
  DISPLAY_VUMETER
};

display_type current_display_type = DISPLAY_TEXT;

#define TRIGGER_PIN 33

/*******************************
 * Setup
 *******************************/
void setup(void)
{
  // Start the serial port
  Serial.begin(115200);
  delay(SECOND);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  // Start the neopixel
  npsetup();

  WiFiManager wm;

  bool res;

  // wm.resetSettings();

  res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap
  if (!res)
  {

    Serial.println("Failed to connect");

    ESP.restart();
  }

  // Start the WiFi
  // WiFi.mode(WIFI_STA);
  WiFi.hostname(device);
  strcpy((char *)ssid, WiFi.SSID().c_str());
  // WiFi.begin(ssid, password);
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

  if (MDNS.begin(device))
  {
    Serial.printf("MDNS responder started with name %s.local\n", device);
  }

  // Setup for VUmeter display
  // for WS2812B - we're using 64x32 RGB Matrix
  // FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds[0], NUM_LEDS);
  // Serial.begin(57600);

  setupWebServer();
  setupAudio();

  if (M_WIDTH == 8)
    numBands = 8;
  else
    numBands = 16;
  barWidth = M_WIDTH / numBands;

  EEPROM.begin(EEPROM_SIZE);

  // It should not normally be possible to set the gain to 255
  // If this has happened, the EEPROM has probably never been written to
  // (new board?) so reset the values to something sane.
  if (EEPROM.read(EEPROM_GAIN) == 255) {
    EEPROM.write(EEPROM_BRIGHTNESS, 50);
    EEPROM.write(EEPROM_GAIN, 0);
    EEPROM.write(EEPROM_SQUELCH, 0);
    EEPROM.write(EEPROM_PATTERN, 0);
    EEPROM.write(EEPROM_DISPLAY_TIME, 10);
    EEPROM.commit();
  }

  // Read saved values from EEPROM
  // FastLED.setBrightness( EEPROM.read(EEPROM_BRIGHTNESS));
  // brightness = FastLED.getBrightness();
  gain = EEPROM.read(EEPROM_GAIN);
  squelch = EEPROM.read(EEPROM_SQUELCH);
  pattern = EEPROM.read(EEPROM_PATTERN);
  displayTime = EEPROM.read(EEPROM_DISPLAY_TIME);

  // timer.every(2 * SECOND, readtemp);
  timer.every(0.5 * SECOND, npblink);
  timer.every(speed, display);
  timer.every(3 * SECOND, wmloop);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { MainPage(request); }); /*Client request handling: calls the function to serve HTML page */

  server.on("/inline", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "this works as well"); });

  server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request)
            { MainPageSubmit(request); });
  // server.on("/submit", HTTP_POST, []()

  server.on("/LEDupdate", HTTP_GET, [](AsyncWebServerRequest *request)
            { LEDControl(request); });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println("Starting MatrixPanel_I2S_DMA test...");
  // Custom pin mapping for all pins
  HUB75_I2S_CFG::i2s_pins _pins = {R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK};

  HUB75_I2S_CFG mxconfig(
      PANEL_WIDTH,  // width
      PANEL_HEIGHT, // height
      CHAIN_LENGTH, // chain length
      _pins         // pin mapping
  );

  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(255); // 0-255
  dma_display->clearScreen();

  // Clear the layers
  gfx_layer_fg.clear();
  gfx_layer_fg.setTextWrap(false);

  gfx_layer_bg.clear();

  // set up initial messages
  // TODO get SSID from WiFiManager
  lower_msg = ssid;
  upper_msg = "Is this working?";
  top_msg = "MTG";
  Serial.printf("lower_msg = %s, upper_msg = %s \n", lower_msg.c_str(), upper_msg.c_str());
}

/*******************************
 * Loop
 *******************************/
void loop(void)
{
  // server.handleClient();
  timer.tick();
}

/*******************************
 * Utility Functions - WiFi Manager
 *******************************/
bool wmloop(void *)
{
  // is configuration portal requested?
  if (digitalRead(TRIGGER_PIN) == LOW)
  {
    WiFiManager wm;

    // reset settings - for testing
    wm.resetSettings();

    ESP.restart();
  }
  return true;
}

/*******************************
 * Utility Functions - URL Decode
 *******************************/

String urlDecode(String input)
{
  String decoded = "";
  char a, b;
  unsigned int len = input.length();

  for (unsigned int i = 0; i < len; i++)
  {
    char c = input[i];

    if (c == '+')
    {
      decoded += ' ';
    }
    else if (c == '%')
    {
      if (i + 2 < len)
      {
        a = input[i + 1];
        b = input[i + 2];

        // Convert hex digits to decimal
        if (a >= '0' && a <= '9')
          a = a - '0';
        else if (a >= 'a' && a <= 'f')
          a = a - 'a' + 10;
        else if (a >= 'A' && a <= 'F')
          a = a - 'A' + 10;

        if (b >= '0' && b <= '9')
          b = b - '0';
        else if (b >= 'a' && b <= 'f')
          b = b - 'a' + 10;
        else if (b >= 'A' && b <= 'F')
          b = b - 'B' + 10;

        decoded += char(16 * a + b);
        i += 2;
      }
      else
      {
        decoded += '%';
      }
    }
    else
    {
      decoded += c;
    }
  }

  return decoded;
}
/*******************************
 * Utility Functions - Display
 *******************************/

bool display(void *) // Select which display to show
{
  switch (current_display_type)
  {
  case DISPLAY_TEXT:
    text_display();
    break;
  case DISPLAY_VUMETER:
    // vumeter_display();
    break;
  }

  return true;
}

void text_display()
{
  updateBackground();
  scrollText(wheelval, upper_msg.c_str());                   // Prints Scrolling text with a rainbow color
  printTextRainbowCentered(wheelval, lower_msg.c_str(), 25); // Prints text X-Centered to chosen Y position with rainbow color
  printTextRainbowCentered(wheelval, top_msg.c_str(), 0);    // Prints text X-Centered to chosen Y position with rainbow color
  // printTextRainbowCentered(wheelval, lower_msg.c_str(), 24);  //Prints text X-Centered to chosen Y position with rainbow color
  // scrollText(wheelval, lower_msg.c_str());  //Prints text X-Centered to chosen Y position with rainbow color

  // gfx_layer_fg.display();
  // gfx_layer_bg.display();

  gfx_compositor.Blend(gfx_layer_bg, gfx_layer_fg); // blend and immediately display

  wheelval += 1;
  // update timer interval in case it changed
  timer.every(100 / speed, display);
}

void vumeter_display()
{
  updateBackground();
  // TODO implement VU meter display
}

/*******************************
 * Web Server Handlers
 *******************************/
void handleNotFound(AsyncWebServerRequest *request)
{

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";
  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }
  request->send(404, "text/plain", message);
  Serial.println(message);
}
void MainPage(AsyncWebServerRequest *request)
{
  String _html_page = html_page;               /*Read The HTML Page*/
  request->send(200, "text/html", _html_page); /*Send the code to the web server*/
}

void MainPageSubmit(AsyncWebServerRequest *request)
{

  String upperText = request->arg("upperText");
  String lowerText = request->arg("lowerText");
  String topText = request->arg("topText");

  speed = request->arg("speed").toInt() * 10;
  background = request->arg("background").toInt();

  // Decode the URL-encoded strings
  String upperText2 = urlDecode(upperText);
  String lowerText2 = urlDecode(lowerText);
  String topText2 = urlDecode(topText);

  Serial.println(" ***************************");
  Serial.print("Received upperText: ");
  Serial.println(upperText2);
  Serial.print("Received lowerText: ");
  Serial.println(lowerText2);

  Serial.print("Received topText: ");
  Serial.println(topText2);
  Serial.print("Received speed: ");
  Serial.println(speed);
  Serial.print("Received background: ");
  Serial.println(background);

  // // Now use upperText and lowerText - they should have the correct characters
  // Serial.println(upperText2);
  // Serial.println(lowerText2);

  request->send(200, "text/plain", "Messages received!");

  // Clear first, then assign with explicit std::string construction
  upper_msg.clear();
  lower_msg.clear();
  top_msg.clear();

  upper_msg = std::string(upperText2.c_str());
  lower_msg = std::string(lowerText2.c_str());
  top_msg = std::string(topText2.c_str());
}

void LEDControl(AsyncWebServerRequest *request)
{
  // Stop the LED from blinking
  blink = false;
  Serial.println("LED Control activated");
  String message = "LED Control activated\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";
  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }
  Serial.print(message);
  uint8_t LEDred = request->arg("red").toInt();     // Convert and read the LED red
  uint8_t LEDgreen = request->arg("green").toInt(); // Convert and read the LED green
  uint8_t LEDblue = request->arg("blue").toInt();   // Convert and read the LED blue
  // theaterChase(strip.Color(LEDred, LEDgreen, LEDblue), 50); // Set the LED colour
  strip.setPixelColor(0, strip.Color(LEDred, LEDgreen, LEDblue));
  Serial.printf("Red %d, Green %d, Blue %d", LEDred, LEDgreen, LEDblue);
  strip.show();

  request->send(200, "text/plane", message); // Send the LED status to the web server
}

/*******************************
 * Utility Functions - Display
 ********************************/

//------------------------------------------------------------------------------------------------------------------
// The layers don't draw to hardware directly, they use a callback function.
// You could be smart here and additionally draw to the VirtualMatrixPanel_T class...
void layer_draw_callback(int16_t x, int16_t y, uint8_t r_data, uint8_t g_data, uint8_t b_data)
{

  dma_display->drawPixelRGB888(x, y, r_data, g_data, b_data);
}

//------------------------------------------------------------------------------------------------------------------
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8

uint16_t colorWheel(uint8_t pos)
{
  if (pos < 85)
  {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  }
  else if (pos < 170)
  {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  }
  else
  {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}
void printTextRainbowCentered(int colorWheelOffset, const char *text, int yPos)
{
  gfx_layer_fg.setTextSize(1); // size 1 == 8 pixels high

  // Calculate the width of the text in pixels
  int textWidth = strlen(text) * 6; // Assuming 6 pixels per character for size 1

  // Center the text horizontally
  int xPos = (gfx_layer_fg.width() - textWidth) / 2;

  gfx_layer_fg.setCursor(xPos, yPos); // Set cursor position for centered text
                                      // Clear the area of text to be drawn to
  gfx_layer_fg.drawRect(0, yPos, gfx_layer_fg.width(), 8, gfx_layer_fg.color565(0, 0, 0));
  gfx_layer_fg.fillRect(0, yPos, gfx_layer_fg.width(), 8, gfx_layer_fg.color565(0, 0, 0));

  // Draw text with a rotating color
  for (uint8_t w = 0; w < strlen(text); w++)
  {
    gfx_layer_fg.setTextColor(colorWheel((w * 32) + colorWheelOffset));
    gfx_layer_fg.print(text[w]);
  }
}

// Code taken from: https://github.com/witnessmenow/ESP32-Trinity/blob/master/examples/BuildingBlocks/Text/ScrollingText/ScrollingText.ino
//
void scrollText(int colorWheelOffset, const char *text)
{

  const char *str = text;
  byte offSet = 25;
  unsigned long now = millis();
  if (now > isAnimationDue)
  {

    gfx_layer_fg.setTextSize(2); // size 2 == 16 pixels high

    isAnimationDue = now + delayBetweeenAnimations;
    textXPosition -= 1;

    // Checking is the very right of the text off screen to the left
    gfx_layer_fg.getTextBounds(str, textXPosition, textYPosition, &xOne, &yOne, &w, &h);
    if (textXPosition + w <= 0)
    {
      textXPosition = gfx_layer_fg.width() + offSet;
    }

    gfx_layer_fg.setCursor(textXPosition, textYPosition);

    // Clear the area of text to be drawn to
    gfx_layer_fg.drawRect(0, textYPosition, gfx_layer_fg.width(), 16, gfx_layer_fg.color565(0, 0, 0));
    gfx_layer_fg.fillRect(0, textYPosition, gfx_layer_fg.width(), 16, gfx_layer_fg.color565(0, 0, 0));

    uint8_t w = 0;
    for (w = 0; w < strlen(str); w++)
    {
      gfx_layer_fg.setTextColor(colorWheel((w * 32) + colorWheelOffset));
      // gfx_layer_fg.setTextColor(gfx_layer_fg.color565(255, 255, 255));
      gfx_layer_fg.print(str[w]);
    }
  }
}
void drawTextCentered(int colorWheelOffset, const char *text, int yPos)
{

  // draw text with a rotating colour
  gfx_layer_fg.setTextSize(1);      // size 1 == 8 pixels high
                                    // Calculate the width of the text in pixels
  int textWidth = strlen(text) * 6; // Assuming 6 pixels per character for size 1

  // Center the text horizontally
  int xPos = (gfx_layer_fg.width() - textWidth) / 2;

  gfx_layer_fg.setCursor(xPos, yPos); // start at top left, with 8 pixel of spacing
  uint8_t w = 0;

  //   // Clear the area of text to be drawn to
  // gfx_layer_fg.drawRect(0, yPos, gfx_layer_fg.width() , 16, gfx_layer_fg.color565(0, 0, 0));
  // gfx_layer_fg.fillRect(0, yPos, gfx_layer_fg.width() , 16, gfx_layer_fg.color565(0, 0, 0));

  // const char *str = "ESP32 DMA";
  const char *str = text;
  for (w = 0; w < strlen(str); w++)
  {
    gfx_layer_fg.setTextColor(colorWheel((w * 32) + colorWheelOffset));
    gfx_layer_fg.print(str[w]);
  }
}

void updateBackground()
{

  CRGBPalette16 currentPalette = CloudColors_p;

  if ((millis() - last_increment) > 20)
  {
    increment_amt++;
    last_increment = millis();
  }

  for (int x = 0; x < gfx_layer_bg.width(); x++)
  {
    for (int y = 0; y < gfx_layer_bg.height(); y++)
    {

      int val = sin8(x + increment_amt);
      val += sin8(y + increment_amt);

      CRGB currentColor = ColorFromPalette(currentPalette, val); //, brightness, currentBlendType);

      // Set a pixel in the back layer
      gfx_layer_bg.setPixel(x, y, currentColor.r, currentColor.g, currentColor.b);
    }
  }

  gfx_layer_bg.dim(background); // darken it a little
}

/**************************
 * VUmeter display functions
 ***************************/
