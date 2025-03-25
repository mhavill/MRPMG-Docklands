# Let's Chat -ESP32

## ESP32 Server

We are going to set up an ESP32 with a couple of functions as a web server.

This includes using [mDNS](https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html) to give the device a memorable name and save us from needing the IP address

The initial functions will use [HTTP Methods](https://www.w3schools.com/tags/ref_httpmethods.asp):

GET to get the current temperature measured on the [Dallas DS1820](https://www.electronicwings.com/esp32/ds18b20-sensor-interfacing-with-esp32)

PUT to change the [Neopixel LED](https://core-electronics.com.au/guides/ws2812-addressable-leds-arduino-quickstart-guide/)

You can use a device browser to send the HTTP to test the responses.
