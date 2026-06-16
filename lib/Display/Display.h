#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <MClasses.h>

class Display {
    public:
        Display(uint8_t address, uint8_t columns, uint8_t rows, uint8_t lcdBrightPin) : address(address), columns(columns), rows(rows), lcdBrightPin(lcdBrightPin) {}
        void init();
        void clear();
        void print(String *str, uint8_t x, uint8_t y);
        void write(char value, uint8_t x, uint8_t y);
        void setBrightness(uint8_t value);
        void drawDigit(byte dig, byte x, byte y);
    private:
        const uint8_t address;
        const uint8_t lcdBrightPin;
        const uint8_t columns;
        const uint8_t rows;
        
        LiquidCrystal_I2C lcd = LiquidCrystal_I2C(address, columns, rows);

        uint8_t LT[8] = {0b00111,  0b01111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
        uint8_t UB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};
        uint8_t RT[8] = {0b11100,  0b11110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
        uint8_t LL[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b01111,  0b00111};
        uint8_t LB[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
        uint8_t LR[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11110,  0b11100};
        uint8_t UMB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
        uint8_t LMB[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
};