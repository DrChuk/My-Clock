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

void showTime(String &time, String &date, bool isAmFormat) {
    static Timer dotsTimer(500);
    static bool dots;

    if (dotsTimer.ready()) dots = !dots;

    uint8_t x = 3;
    uint8_t y = 1;

    String fTime;
    uint8_t hours = (uint8_t)(time[0] - '0') * 10 + (uint8_t)(time[1] - '0');
    uint8_t minutes = (uint8_t)(time[3] - '0') * 10 + (uint8_t)(time[4] - '0');
    String ap;

    if (isAmFormat) {
        ap = (hours > 12) ? "PM" : "AM";
        hours -= 12;
    }
    fTime = ((hours < 10) ? ('0' + String(hours)) : String(hours)) + ':' + ((minutes < 10) ? ('0' + String(minutes)) : String(minutes));
    
    for (uint8_t i = 0; i < fTime.length(); i++) {
        if (fTime[i] == ':') {
            if (dots) {
                display.write('.', x - 1, y);
                display.write('.', x - 1, y + 1);
            }
            else {
                display.write(' ', x - 1, y);
                display.write(' ', x - 1, y + 1);
            }
        }
        else {
            display.drawDigit(fTime[i] - '0', x, y);
            x += 4;
        }
    }

    uint8_t dateX = 10 - (date.length() / 2);

    display.print(&date, dateX, 3);
    display.print(&ap, 18, 2);
}