#include <Arduino.h>
#include <FastLED.h>
#include <Timer.h>

#define RGB_PIN 6
CRGB rgb[1];

class Color {
    public:
        uint8_t red;
        uint8_t green;
        uint8_t blue;
};

Color color;

void ledSetup() {
    FastLED.addLeds<WS2812, RGB_PIN, GRB>(rgb, 1);
    FastLED.setBrightness(0);
}

void setLedRGB(uint8_t red, uint8_t green, uint8_t blue) {
    rgb[0].setRGB(red, green, blue);
    FastLED.show();
}

void setLedHue(uint8_t hue) {
    rgb[0].setHue(hue);
    FastLED.show();
}

uint8_t targetBright = 50;
uint8_t mode;

// Blinking effect settings
uint8_t counts = 4;
uint32_t duration = 50;
uint32_t cd = 500;

Timer durationTimer(duration);
Timer cdTimer(cd);
bool isBlinking = false;
bool isTurnedOn = false;

// Breathe effect settings
uint8_t minBright = 10;
uint8_t maxBright = 50;
Timer stepTimer(45);
uint8_t breatheBright = maxBright;
bool isLowing = true;

void ledLoop() {
    static Timer brightTimer(10);
    static uint8_t currentBright;

    if (brightTimer.ready() && currentBright != targetBright) {
        currentBright = currentBright + (targetBright - currentBright) * 0.05;
        // maxBright = currentBright;
    }

    switch (mode)
    {
        case 0:
            FastLED.setBrightness(currentBright);
            FastLED.show();
            break;
        case 1:
            static uint8_t count;
            if (isBlinking) {
                if (durationTimer.ready()) {
                    if (count >= counts) {
                        count = 0;
                        isBlinking = false;
                    }
                    else {
                        if (!isTurnedOn) {
                            FastLED.setBrightness(currentBright);
                            FastLED.show();
                            isTurnedOn = true;
                        }
                        else {
                            FastLED.setBrightness(0);
                            FastLED.show();
                            isTurnedOn = false;
                            count++;
                        }
                    }
                }
            }
            else {
                if (cdTimer.ready()) {
                    isBlinking = true;
                }
            }
            break;
        case 2:
            if (breatheBright == maxBright && !isLowing) isLowing = true;
            if (breatheBright == minBright && isLowing) isLowing = false;
            if (stepTimer.ready()) {
                if (isLowing) breatheBright--;
                else breatheBright++;
                FastLED.setBrightness(breatheBright);
                FastLED.show();
            }
            break;
    }
}

void setLedMode(uint8_t modeNumber) {
    mode = modeNumber;
}

void setBlinkingSettings(uint8_t countsN, uint32_t durationN, uint32_t cdN) {
    counts = countsN;
    duration = durationN;
    cd = cdN;

    cdTimer.setPeriod(cd);
    durationTimer.setPeriod(duration);
}

void setBreatheDuration(uint8_t value) {
    stepTimer.setPeriod(value);
}

void setLedBrightness(uint8_t bright) {
    targetBright = bright;
}