# 2 Doing Stuff Project

## Introduction

Microcontrollers, like ESP32, are designed to enable the rapid creation of systems which are able to monitor and control devices to provide a complete system.  
For the full [ESP32 Series datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf#:~:text=ESP32%20is%20a%20single%202.4%20GHz%20Wi%2DFi%2Dand%2DBluetooth,(NRND)%2C%20and%20ESP32%2DD0WDQ6%20(NRND)%2C%20among%20which%2C%20%E2%80%A2)

The [ESP32-WROVER from Lonely Binary](https://lonelybinary.com/collections/esp32/products/lonely-binary-esp32-wrover-the-king-of-esp32) we are using is the version WROVER (D0WDQ6) and is mounted on a 'Devkit' board which adds a number of components like power regulation, neopixel LED, serial chip for USB etc.
This makes it very useful as a maker board.
Although different components on the Devkit board this tutorial presents a good understanding of the [ESP32 internals](https://www.circuitschools.com/what-is-esp32-how-it-works-and-what-you-can-do-with-esp32/)

## Goal

Use your chosen IDE to write and load the code to drive the [Neopixel RGB LED](https://cdn-shop.adafruit.com/datasheets/WS2812.pdf) on the board.
The code provided here in the [Arduino folder](ESP\2_Doing_Stuff\Code-Arduino\RGB_strip1-main) or [PlatformIO folder](ESP\2_Doing_Stuff\Code-PIO\RGB-strip1-main) will produce a pleasing coloured rolling display.

We have some 8LED strips which allows you to extend the pattern to all the LEDs
