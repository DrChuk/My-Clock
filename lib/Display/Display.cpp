#include <Arduino.h>
#include "Display.h"

void Display::clear() {
    lcd.clear();
}

void Display::print(String* str, uint8_t x, uint8_t y) {
    lcd.setCursor(x, y);
    lcd.print(*str);
}

void Display::write(char value, uint8_t x, uint8_t y) {
    lcd.setCursor(x, y);
    lcd.write(value);
}

void Display::setBrightness(uint8_t value) {
    analogWrite(lcdBrightPin, value);
}

void Display::init() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    pinMode(lcdBrightPin, OUTPUT);
    setBrightness(255);

    lcd.createChar(0, LT);
    lcd.createChar(1, UB);
    lcd.createChar(2, RT);
    lcd.createChar(3, LL);
    lcd.createChar(4, LB);
    lcd.createChar(5, LR);
    lcd.createChar(6, UMB);
    lcd.createChar(7, LMB);
}

void Display::drawDigit(byte dig, byte x, byte y) {
    switch (dig) {
        case 0:
            lcd.setCursor(x, y); // set cursor to column 0, line 0 (first row)
            lcd.write(0);  // call each segment to create
            lcd.write(1);  // top half of the number
            lcd.write(2);
            lcd.setCursor(x, y + 1); // set cursor to colum 0, line 1 (second row)
            lcd.write(3);  // call each segment to create
            lcd.write(4);  // bottom half of the number
            lcd.write(5);
            break;
        case 1:
            lcd.setCursor(x, y);
            lcd.write(32);
            lcd.write(1);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(32);
            lcd.write(32);
            lcd.write(5);
            break;
        case 2:
            lcd.setCursor(x, y);
            lcd.write(6);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(3);
            lcd.write(7);
            lcd.write(7);
            break;
        case 3:
            lcd.setCursor(x, y);
            lcd.write(6);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(7);
            lcd.write(7);
            lcd.write(5);
            break;
        case 4:
            lcd.setCursor(x, y);
            lcd.write(3);
            lcd.write(4);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(32);
            lcd.write(32);
            lcd.write(5);
            break;
        case 5:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(6);
            lcd.setCursor(x, y + 1);
            lcd.write(7);
            lcd.write(7);
            lcd.write(5);
            break;
        case 6:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(6);
            lcd.setCursor(x, y + 1);
            lcd.write(3);
            lcd.write(7);
            lcd.write(5);
            break;
        case 7:
            lcd.setCursor(x, y);
            lcd.write(1);
            lcd.write(1);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(32);
            lcd.write(0);
            lcd.write(32);
            break;
        case 8:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(3);
            lcd.write(7);
            lcd.write(5);
            break;
        case 9:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(4);
            lcd.write(4);
            lcd.write(5);
            break;
        case 10:
            lcd.setCursor(x, y);
            lcd.write(32);
            lcd.write(32);
            lcd.write(32);
            lcd.setCursor(x, y + 1);
            lcd.write(32);
            lcd.write(32);
            lcd.write(32);
            break;
    }
}
