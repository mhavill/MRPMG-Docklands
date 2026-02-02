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
#include <WebServer.h>
#include <ESPmDNS.h>
#include <arduino-timer.h>
#include <WiFiManager.h>

// #include "secrets.h"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <GFX_Layer.hpp>

// Webpage
#include "landing_page.h"
#include "animations_page.h"
#include "text_message_page.h"
#include "vumeter_page.h"
#include <iostream>
#include <string>

// NeoPixel
#include "neopixel.hpp"

// Filesystem
#include "FS.h"
#include <LittleFS.h>
#include <AnimatedGIF.h>

/*******************************
 * Protptypes
 *******************************/

void handleNotFound();
void setup();
void loop();
String urlDecode(String input);
void MainPage();
void MainPageSubmit();
void LEDControl();
void layer_draw_callback(int16_t x, int16_t y, uint8_t r_data, uint8_t g_data, uint8_t b_data);
uint16_t colorWheel(uint8_t pos);
void printTextRainbowCentered(int colorWheelOffset, const char *text, int yPos);
void scrollText(int colorWheelOffset, const char *text);
void drawTextCentered(int colorWheelOffset, const char *text, int yPos);
void updateBackground();
bool display_text(void *);
bool display_animations(void *);
bool wmloop(void *);
bool server_client_handler(void *);

void GIFDraw(GIFDRAW *pDraw);
void *GIFOpenFile(const char *fname, int32_t *pSize);
void GIFCloseFile(void *pHandle);
int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition);
void ShowGIF(char *name);

/*******************************
 * Definitions
 *******************************/

#define device "ESP32server"
const int SECOND = 1000;
static float tempC;
// static DeviceAddress deviceAddress;
// static bool waitForConversion = false;

WebServer server(80);

const int LED = 2;

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
#define NUM_LEDS PANE_WIDTH *PANE_HEIGHT

#define FILESYSTEM LittleFS
#define FORMAT_LITTLEFS_IF_FAILED true

#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1  // Total number of panels chained one to another horizontally only.

uint16_t colorWheel(uint8_t pos);
char ssid[32] = {0};

//------------------------------------------------------------------------------------------------------------------

MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

AnimatedGIF gif;
File f;
int x_offset, y_offset;

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

enum MODE
{
  MODE_TEXT,
  MODE_ANIMATIONS,
  MODE_VUMETER,
  MODE_STOP
} display_mode;

// select which pin will trigger the configuration portal when set to LOW
#define TRIGGER_PIN 33
/****************************
 * GIF Playback Variables
 ****************************/
String gifDir = "/gifs"; // play all GIFs in this directory on the SD card
char filePath[256] = {0};
File root, gifFile;

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

  timer.every(0.5 * SECOND, npblink);
  timer.in(speed, display_text);
  timer.every(3 * SECOND, wmloop);
  timer.every(10, server_client_handler);

  /***********************************
   * Web Server Handlers
   * ******************************/
  server.on("/", HTTP_GET, []()
            {
  // Serve landing page
  server.send(200, "text/html", landing_page_html);
  Serial.println("Served Landing Page"); });

  server.on("/setmode", HTTP_POST, []()
            {
  String mode = server.arg("mode");
  
  if (mode == "text") 
  {
    display_mode = MODE_TEXT;
    server.send(200, "text/html", text_message_page_html);
    Serial.println("Served Text Message Page");
  } 
  else if (mode == "animations") 
  {
    // Start animations on display
    display_mode = MODE_ANIMATIONS;
    // Start going through GIFS
    gif.begin(LITTLE_ENDIAN_PIXELS);
    gifFile = root.openNextFile();
    server.send(200, "text/html", animations_page_html);
    Serial.println("Served Animations Page");
  } 
  else if (mode == "vumeter") 
  {
    display_mode = MODE_VUMETER;
    server.send(200, "text/html", vumeter_page_html);
    Serial.println("Served VU Meter Page"); 
  } 
  else if (mode == "stop") 
  {
    // Stop current mode
    display_mode = MODE_STOP;
    server.send(200, "text/html", landing_page_html);
    Serial.println("Served Landing Page after STOP");
  } 
  else 
  {
    server.send(400, "text/plain", "Invalid mode");
    Serial.println("Invalid mode received");
  } });
  // server.on("/", MainPage); /*Client request handling: calls the function to serve HTML page */

  // server.on("/inline", []()
  //           { server.send(200, "text/plain", "this works as well"); });

  server.on("/submit", HTTP_POST, MainPageSubmit);
  // server.on("/submit", HTTP_POST, []()

  server.on("/LEDupdate", LEDControl);

  server.onNotFound(handleNotFound);
  /***************************
   * Start Web Server
   * ***************************/
  server.begin();
  Serial.println("HTTP server started");

  /***********************************
   * Initialize Display
   * ******************************/

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
  dma_display->setBrightness8(128); // 0-255
  dma_display->clearScreen();

  // Clear the layers
  gfx_layer_fg.clear();
  gfx_layer_fg.setTextWrap(false);

  gfx_layer_bg.clear();

  // set up initial messages & display mode
  display_mode = MODE_TEXT;
  lower_msg = ssid;
  upper_msg = " Melbourne Tech Guilds!  Please register now and enjoy the event. ";
  top_msg = "Welcome to";
  Serial.printf("lower_msg = %s, upper_msg = %s \n", lower_msg.c_str(), upper_msg.c_str());

  /*******************
   * Initialize LittleFS
   ******************/
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
  {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  Serial.println("LittleFS Mounted Successfully");
}

/*******************************
 * Loop
 *******************************/

void loop(void)
{
  timer.tick();
  switch (display_mode)
  {
  case MODE_TEXT:
    // handled in display() timer
    timer.in(speed, display_text);
    break;
  case MODE_ANIMATIONS:
    timer.in(1 * SECOND, display_animations);

    break;
  case MODE_VUMETER:
    // handled in display() timer
    break;
  case MODE_STOP:
    // Do nothing
    break;
  }
}

/*******************************
 * Utility Functions
 *******************************/
bool display_animations(void *)
{
  if (display_mode != MODE_ANIMATIONS)
    return true; // exit if mode changed
  server.handleClient();
  timer.in(100, display_animations);

  // Play all GIFs in the gifs directory
  root = FILESYSTEM.open(gifDir);
  if (root)
  {
    gifFile = root.openNextFile();
    while (gifFile)
    {
      if (!gifFile.isDirectory()) // play it
      {
        // C-strings... urghh...
        memset(filePath, 0x0, sizeof(filePath));
        strcpy(filePath, gifFile.path());

        // Show it.
        ShowGIF(filePath);
      }
      gifFile.close();
      server.handleClient();
      if (display_mode != MODE_ANIMATIONS)
        return true; // exit if mode changed
      gifFile = root.openNextFile();
    }
    root.close();
  } // root
  return true;
}

bool server_client_handler(void *)
{
  server.handleClient();
  return true;
}

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

  // Serial.println("=== Decode Complete ===");

  return decoded;
}

bool display_text(void *)
{
  timer.in(speed, display_text); // re-schedule next display update
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
  timer.in(100 / speed, display_text);
  return true;
}

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
// void MainPage()
// {
//   String _html_page = html_page;             /*Read The HTML Page*/
//   server.send(200, "text/html", _html_page); /*Send the code to the web server*/
// }

void MainPageSubmit()
{

  String upperText = server.arg("upperText");
  String lowerText = server.arg("lowerText");
  String topText = server.arg("topText");

  speed = server.arg("speed").toInt() * 10;
  background = server.arg("background").toInt();

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

  server.send(200, "text/plain", "Messages received!");

  // Clear first, then assign with explicit std::string construction
  upper_msg.clear();
  lower_msg.clear();
  top_msg.clear();

  upper_msg = std::string(upperText2.c_str());
  lower_msg = std::string(lowerText2.c_str());
  top_msg = std::string(topText2.c_str());
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
  uint8_t LEDred = server.arg(0).toInt();   // Convert and read the LED red
  uint8_t LEDgreen = server.arg(1).toInt(); // Convert and read the LED green
  uint8_t LEDblue = server.arg(2).toInt();  // Convert and read the LED blue
  // theaterChase(strip.Color(LEDred, LEDgreen, LEDblue), 50); // Set the LED colour
  strip.setPixelColor(0, strip.Color(LEDred, LEDgreen, LEDblue));
  Serial.printf("Red %d, Green %d, Blue %d", LEDred, LEDgreen, LEDblue);
  strip.show();

  server.send(200, "text/plane", message); // Send the LED status to the web server
}

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

/***********************
 * Gif File Handling
 **********************/
// Draw a line of image directly on the LED Matrix
void GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth > dma_display->width())
    iWidth = dma_display->width();

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) // restore to background color
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }
  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency) // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int x, iCount;
    pEnd = s + pDraw->iWidth;
    x = 0;
    iCount = 0; // count non-transparent pixels
    while (x < pDraw->iWidth)
    {
      c = ucTransparent - 1;
      d = usTemp;
      while (c != ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent) // done, stop
        {
          s--; // back up to treat it like transparent
        }
        else // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      } // while looking for opaque pixels
      if (iCount) // any opaque pixels?
      {
        for (int xOffset = 0; xOffset < iCount; xOffset++)
        {
          dma_display->drawPixel(x + xOffset, y, usTemp[xOffset]); // 565 Color Format
        }
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          iCount++;
        else
          s--;
      }
      if (iCount)
      {
        x += iCount; // skip these
        iCount = 0;
      }
    }
  }
  else // does not have transparency
  {
    s = pDraw->pPixels;
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    for (x = 0; x < pDraw->iWidth; x++)
    {
      dma_display->drawPixel(x, y, usPalette[*s++]); // color 565
    }
  }
} /* GIFDraw() */

void *GIFOpenFile(const char *fname, int32_t *pSize)
{
  Serial.print("Playing gif: ");
  Serial.println(fname);
  f = FILESYSTEM.open(fname);
  if (f)
  {
    *pSize = f.size();
    return (void *)&f;
  }
  return NULL;
} /* GIFOpenFile() */

void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
    f->close();
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  int32_t iBytesRead;
  iBytesRead = iLen;
  File *f = static_cast<File *>(pFile->fHandle);
  // Note: If you read a file all the way to the last byte, seek() stops working
  if ((pFile->iSize - pFile->iPos) < iLen)
    iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
  if (iBytesRead <= 0)
    return 0;
  iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
  pFile->iPos = f->position();
  return iBytesRead;
} /* GIFReadFile() */

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  i = micros() - i;
  //  Serial.printf("Seek time = %d us\n", i);
  return pFile->iPos;
} /* GIFSeekFile() */

unsigned long start_tick = 0;

void ShowGIF(char *name)
{
  start_tick = millis();

  if (gif.open(name, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
  {
    x_offset = (dma_display->width() - gif.getCanvasWidth()) / 2;
    if (x_offset < 0)
      x_offset = 0;
    y_offset = (dma_display->height() - gif.getCanvasHeight()) / 2;
    if (y_offset < 0)
      y_offset = 0;
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    Serial.flush();
    while (gif.playFrame(true, NULL))
    {
      if ((millis() - start_tick) > 8000)
      { // we'll get bored after about 8 seconds of the same looping gif
        break;
      }
    }
    gif.close();
  }

} /* ShowGIF() */

/***********************
 * End of File
 **********************/