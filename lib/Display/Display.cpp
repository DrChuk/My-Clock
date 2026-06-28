#include "Display.h"

// const uint8_t LT[8] PROGMEM = {0b00111,  0b01111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// const uint8_t UB[8] PROGMEM = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};
// const uint8_t RT[8] PROGMEM = {0b11100,  0b11110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
// const uint8_t LL[8] PROGMEM = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b01111,  0b00111};
// const uint8_t LB[8] PROGMEM = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
// const uint8_t LR[8] PROGMEM = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11110,  0b11100};
// const uint8_t UM[8] PROGMEM = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
// const uint8_t LM[8] PROGMEM = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};

const uint8_t CHARS[8][8] PROGMEM = {
    {0b00111, 0b01111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111},
    {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    {0b11100, 0b11110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111},
    {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b01111, 0b00111},
    {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111},
    {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11110, 0b11100},
    {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111},
    {0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111},
};

const uint8_t DIGIT_SEGMENTS[11][6] PROGMEM = {
    {0, 1, 2, 3, 4, 5},
    {32, 1, 2, 32, 32, 5},
    {6, 6, 2, 3, 7, 7},
    {6, 6, 2, 7, 7, 5},
    {3, 4, 2, 32, 32, 5},
    {0, 6, 6, 7, 7, 5},
    {0, 6, 6, 3, 7, 5},
    {1, 1, 2, 32, 0, 32},
    {0, 6, 2, 3, 7, 5},
    {0, 6, 2, 4, 4, 5},
    {32, 32, 32, 32, 32, 32}
};

Display::Display(uint8_t displayAddress, uint8_t brightPin) : lcd(displayAddress, 20, 4) {
    address = displayAddress;
    brightnessPin = brightPin;
    
    memset(data, ' ', sizeof(data));
    memset(oldData, ' ', sizeof(oldData));
}

void Display::init() {
    lcd.init();
    lcd.backlight();
    lcd.clear();

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t buf[8];
        memcpy_P(buf, CHARS[i], 8);
        lcd.createChar(i, buf);
    }
}

void Display::print(const char* value, uint8_t x, uint8_t y) {
    uint8_t xPos = x, yPos = y;
    for (uint8_t i = 0; value[i] != '\0'; i++) {
        setValue(value[i], xPos, yPos);
        if (++xPos >= width) { xPos = 0; yPos++; }
        if (yPos >= height) yPos = 0;
    }
}

void Display::print(const __FlashStringHelper* value, uint8_t x, uint8_t y) {
    uint8_t xPos = x, yPos = y;
    PGM_P p = reinterpret_cast<PGM_P>(value);
    for (uint8_t i = 0; pgm_read_byte(&p[i]) != '\0' && i < (width * height); i++) {
        setValue(pgm_read_byte(&p[i]), xPos, yPos);
        if (++xPos >= width) { xPos = 0; yPos++; }
        if (yPos >= height) yPos = 0;
    }
}

void Display::show() {
    for (uint8_t j = 0; j < height; j++) {
        for (uint8_t i = 0; i < width; i++) {
            uint8_t charAddress = j * width + i;
            uint8_t current = data[charAddress];
            if (current != oldData[charAddress]) {
                lcd.setCursor(i, j);
                lcd.write(current);
                oldData[charAddress] = current;
            }
        }
    }
}

void Display::clear() {
    for (uint8_t i = 0; i < width * height; i++) {
        data[i] = ' ';
    }
}

void Display::setBrightness(uint8_t value) {
    analogWrite(brightnessPin, value);
}

void Display::drawDigit(uint8_t digit, uint8_t x, uint8_t y) {
    if (digit > 10) return;
    for (uint8_t i = 0; i < 6; i++) {
        uint8_t segment = pgm_read_byte(&DIGIT_SEGMENTS[digit][i]);
        setValue(segment, x + i % 3, y + i / 3);
    }
}