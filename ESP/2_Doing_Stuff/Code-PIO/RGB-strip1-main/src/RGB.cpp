#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


uint32_t Wheel(byte WheelPos);
int LedCount = 8;
int GPIOPIN = 2;
neoPixelType type = NEO_GRB + NEO_KHZ800;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LedCount, GPIOPIN, type);


void setup()
{
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(50);
  strip.show();
}

void loop()
{
  for (int i = 0; i <= 255; i++)
  {
    for (int j = 0; j < LedCount; j++)
    {
      strip.setPixelColor(j, Wheel(i));
      strip.show();
      delay(10);
    }
  }
}

uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color((255 - WheelPos * 3) / 2, 0, (WheelPos * 3) / 2);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, (WheelPos * 3) / 2, (255 - WheelPos * 3) / 2);
  }
  WheelPos -= 170;
  return strip.Color((WheelPos * 3) / 2, (255 - WheelPos * 3) / 2, 0);
}
