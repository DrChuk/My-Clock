#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Display {
    public:
        Display(uint8_t displayAddress, uint8_t brightPin);
        void init();
        void clear();
        inline void setValue(uint8_t value, uint8_t x, uint8_t y) {
            if (x < width && y < height) data[y * width + x] = value;
        }
        inline uint8_t getValue(uint8_t x, uint8_t y) {
            return (x < width && y < height) ? data[y * width + x] : ' ';
        }
        void drawDigit(uint8_t digit, uint8_t x, uint8_t y);
        void print(const char* value, uint8_t x, uint8_t y);
        void print(const __FlashStringHelper* value, uint8_t x, uint8_t y);
        void setBrightness(uint8_t value);
        void show();
    private:
        uint8_t address, brightnessPin;
        const uint8_t width = 20;
        const uint8_t height = 4;
        char data[20 * 4];
        char oldData[20 * 4];

        LiquidCrystal_I2C lcd;
        // LiquidCrystal_I2C lcd = LiquidCrystal_I2C(address, width, height);
};