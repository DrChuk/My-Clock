#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Display {
    public:
        Display(uint8_t displayAddress, uint8_t cols, uint8_t r, uint8_t brightPin);
        ~Display();
        void init();
        void clear();
        void setValue(uint8_t value, uint8_t x, uint8_t y);
        void drawDigit(uint8_t digit, uint8_t x, uint8_t y);
        void showTime(String* time, String* date, uint8_t dots);
        void print(String value, uint8_t x, uint8_t y);
        void setBrightness(uint8_t value);
        void show();
        uint8_t getValue(uint8_t x, uint8_t y);
    private:
        uint8_t width, height, address, brightnessPin;
        int* data;
        int* oldData;

        LiquidCrystal_I2C lcd = LiquidCrystal_I2C(address, width, height);
};