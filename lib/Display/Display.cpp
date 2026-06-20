#include "Display.h"
#include <LiquidCrystal_I2C.h>

Display::Display(uint8_t displayAddress, uint8_t w, uint8_t h, uint8_t brightPin) {
    width = w;
    height = h;
    address = displayAddress;
    brightnessPin = brightPin;
    data = new int[w * h];
    oldData = new int[w * h];
    
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            oldData[j * w + i] = ' ';
        }
    }

    lcd = LiquidCrystal_I2C(address, w, h);
}

void Display::init() {
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.createChar(0, LT);
    lcd.createChar(1, UB);
    lcd.createChar(2, RT);
    lcd.createChar(3, LL);
    lcd.createChar(4, LB);
    lcd.createChar(5, LR);
    lcd.createChar(6, UMB);
    lcd.createChar(7, LMB);
}

void Display::setValue(uint8_t value, uint8_t x, uint8_t y) {
    data[y * width + x] = value;
}

uint8_t Display::getValue(uint8_t x, uint8_t y) {
    return data[y * width + x];
}

void Display::print(String value, uint8_t x, uint8_t y) {
    uint8_t xPos = x;
    uint8_t yPos = y;
    for (int i = 0; i < value.length(); i++) {
        if (i < (width * height)) {
            setValue(value[i], xPos, yPos);
            xPos++;
            if (xPos > width) {
                xPos = 0;
                yPos++;
            }
            if (yPos > height) {
                yPos = 0;
            }
        }
    }
}

void Display::show() {
    // lcd.home();
    // lcd.clear();
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (oldData[j * width + i] != getValue(i, j)) {
                lcd.setCursor(i, j);
                lcd.write(getValue(i, j));
                oldData[j * width + i] = getValue(i, j);
            }
        }
    }
}

void Display::clear() {
    for (int i = 0; i < width * height; i++) {
        data[i] = ' ';
    }
}

Display::~Display() {
    delete[] data;
}

void Display::setBrightness(uint8_t value) {
    analogWrite(brightnessPin, value);
}

void Display::drawDigit(uint8_t digit, uint8_t x, uint8_t y) {
    switch (digit) {
        case 0:
            setValue(0, x, y);
            setValue(1, x + 1, y);
            setValue(2, x + 2, y);
            setValue(3, x, y + 1);
            setValue(4, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 1:
            setValue(32, x, y);
            setValue(1, x + 1, y);
            setValue(2, x + 2, y);
            setValue(32, x, y + 1);
            setValue(32, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 2:
            setValue(6, x, y);
            setValue(6, x + 1, y);
            setValue(2, x + 2, y);
            setValue(3, x, y + 1);
            setValue(7, x + 1, y + 1);
            setValue(7, x + 2, y + 1);
            break;
        case 3:
            setValue(6, x, y);
            setValue(6, x + 1, y);
            setValue(2, x + 2, y);
            setValue(7, x, y + 1);
            setValue(7, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 4:
            setValue(3, x, y);
            setValue(4, x + 1, y);
            setValue(2, x + 2, y);
            setValue(32, x, y + 1);
            setValue(32, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 5:
            setValue(0, x, y);
            setValue(6, x + 1, y);
            setValue(6, x + 2, y);
            setValue(7, x, y + 1);
            setValue(7, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 6:
            setValue(0, x, y);
            setValue(6, x + 1, y);
            setValue(6, x + 2, y);
            setValue(3, x, y + 1);
            setValue(7, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 7:
            setValue(1, x, y);
            setValue(1, x + 1, y);
            setValue(2, x + 2, y);
            setValue(32, x, y + 1);
            setValue(0, x + 1, y + 1);
            setValue(32, x + 2, y + 1);
            break;
        case 8:
            setValue(0, x, y);
            setValue(6, x + 1, y);
            setValue(2, x + 2, y);
            setValue(3, x, y + 1);
            setValue(7, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 9:
            setValue(0, x, y);
            setValue(6, x + 1, y);
            setValue(2, x + 2, y);
            setValue(4, x, y + 1);
            setValue(4, x + 1, y + 1);
            setValue(5, x + 2, y + 1);
            break;
        case 10:
            setValue(32, x, y);
            setValue(32, x + 1, y);
            setValue(32, x + 2, y);
            setValue(32, x, y + 1);
            setValue(32, x + 1, y + 1);
            setValue(32, x + 2, y + 1);
            break;
    }
}