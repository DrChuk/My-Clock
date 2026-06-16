#include <Arduino.h>
#include <FastLED.h>

#define RGB_PIN 6
CRGB rgb[1];

void ledSetup() {
    FastLED.addLeds<WS2812, RGB_PIN, GRB>(rgb, 1);
    FastLED.setBrightness(50);
}

void setLedRGB(uint8_t red, uint8_t green, uint8_t blue) {
    rgb[0].setRGB(red, green, blue);
    FastLED.show();
}

void setLedHue(uint8_t hue) {
    rgb[0].setHue(hue);
    FastLED.show();
}

void setLedBrightness(uint8_t bright) {
    FastLED.setBrightness(bright);
}