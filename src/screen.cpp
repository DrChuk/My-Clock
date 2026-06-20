#include <Arduino.h>
#include <Display.h>
#include <Timer.h>

Display display(0x27, 20, 4, 3);

// RTC functions
void getTime(String *buffer, bool sec);
void getDate(String *buffer, bool withWeekday);

void screenSetup() {
    display.init();
}

void test() {
    display.clear();
    // display.setValue('H', 0, 0);
    display.setValue('H', 0, 0);
    display.setValue('H', 1, 1);
    display.setValue('H', 2, 2);
    display.setValue('H', 3, 3);
    // display.setValue('H', 2, 0);
    // display.setValue('H', 0, 2);
    // display.setValue('H', 0, 3);
    // display.setValue('H', 1, 0);
    // display.setValue('H', 1, 1);
    // display.setValue('H', 1, 2);
    // display.setValue('H', 1, 3);
    // display.drawDigit(3, 0, 0);
    display.show();
}

void showTime(String &time, String &date, bool isAmFormat) {
    static Timer dotsTimer(500);
    static bool dots;

    display.clear();

    if (dotsTimer.ready()) dots = !dots;

    uint8_t x = 3;
    uint8_t y = 1;

    String fTime;
    uint8_t hours = (uint8_t)(time[0] - '0') * 10 + (uint8_t)time[1] - '0';
    uint8_t minutes = (uint8_t)(time[3] - '0') * 10 + (uint8_t)(time[4] - '0');
    String ap;

    if (isAmFormat) {
        ap = (hours > 12) ? "PM" : "AM";
        if (hours > 12) hours -= 12;
    }
    fTime = ((hours < 10) ? ('0' + String(hours)) : String(hours)) + ':' + ((minutes < 10) ? ('0' + String(minutes)) : String(minutes));

    for (uint8_t i = 0; i < fTime.length(); i++) {
        if (fTime[i] == ':') {
            if (dots) {
                display.setValue(165, x - 1, y);
                display.setValue(165, x - 1, y + 1);
            }
            else {
                display.setValue(' ', x - 1, y);
                display.setValue(' ', x - 1, y + 1);
            }
        }
        else {
            display.drawDigit(fTime[i] - '0', x, y);
            x += 4;
        }
    }

    uint8_t dateX = 10 - (date.length() / 2);

    display.print(date, dateX, 3);
    display.print(ap, 18, 2);
    display.show();
}

void showSensorData(float temp, float pressure) {
    // String tempS = String(temp);
    // String pressureS = String(pressure);

    String value = String(temp) + char(223) + "C, " + String(pressure) + "mmHg";

    display.clear();

    uint8_t x = 0;
    uint8_t y = 0;

    for (int i = 0; i < String(temp).length() - 1; i++) {
        if (String(temp)[i] == '.') {
            display.setValue('.', x - 1, y + 1);
        }
        else {
            display.drawDigit((uint8_t)(String(temp)[i] - '0'), x, y);
            x += 4;
        }
    }
    display.setValue(111, x - 1, y);
    display.setValue('C', x, y);

    x = 7;
    y = 2;

    for (int i = 0; i < String(pressure).length() - 3; i++) {
        display.drawDigit((uint8_t)(String(pressure)[i]) - '0', x, y);
        x += 4;
    }

    display.print("mm", x - 1, y);
    display.print("Hg", x - 1, y + 1);

    display.show();
}

uint8_t targetValue;

void screenLoop() {
    static Timer timer(10);

    static uint8_t current;

    if (timer.ready() && current != targetValue) {
        current = current + (targetValue - current) * 0.05;
        display.setBrightness(current);
    }
}

void setScreenBright(uint8_t value) {
    targetValue = value;
}

void clearScreen() {
    display.clear();
}