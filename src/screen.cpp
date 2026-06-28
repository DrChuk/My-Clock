#include <Arduino.h>
#include <Display.h>
#include <Timer.h>

Display display(0x27, 3);

void screenSetup() {
    display.init();
}

void showTime(const char* time, const char* date, bool isAmFormat) {
    static Timer dotsTimer(500);
    static bool dots;

    display.clear();

    if (dotsTimer.ready()) dots = !dots;

    uint8_t x = 3;
    uint8_t y = 1;

    String fTime;
    uint8_t hours = (uint8_t)(time[0] - '0') * 10 + (uint8_t)time[1] - '0';
    uint8_t minutes = (uint8_t)(time[3] - '0') * 10 + (uint8_t)(time[4] - '0');
    char ap[2];

    if (isAmFormat) {
        ap[0] = (hours > 12) ? 'P' : 'A';
        ap[1] = 'M';
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

    uint8_t dateX = 10 - (sizeof(date) / sizeof(date[0]) / 2);

    display.print(date, dateX, 3);
    display.print(ap, 18, 2);
    display.show();
}

void showSensorData(float temp, float pressure) {
    display.clear();

    uint8_t x = 0;
    uint8_t y = 0;

    for (unsigned int i = 0; i < String(temp).length() - 1; i++) {
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

    for (unsigned int i = 0; i < String(pressure).length() - 3; i++) {
        display.drawDigit((uint8_t)(String(pressure)[i]) - '0', x, y);
        x += 4;
    }

    display.print(F("mm"), x - 1, y);
    display.print(F("Hg"), x - 1, y + 1);

    display.show();
}

void showDefault() {
    display.clear();
    display.print(F("Default"), 6, 0);
    display.print(F("Default"), 6, 1);
    display.print(F("Default"), 6, 2);
    display.print(F("Default"), 6, 3);
    display.show();
}

void showMenu(uint8_t menuState) {
    display.clear();

    display.print(F("Timer"), 1, 0);
    display.print(F("Alarm"), 1, 1);
    display.print(F("Stopwatch"), 1, 2);
    display.print(F("Settings"), 1, 3);
    
    display.setValue('>', 0, menuState);
    
    display.show();
}

void showTimer(uint8_t selected, String time, bool isTicking, bool isPause, bool isShow) {
    String options = "bhmsp";

    display.clear();
    display.print("Back", 1, 0);

    display.setValue(time[0], 0, 2);

    uint8_t x = 1;
    uint8_t y = 1;
    for (unsigned int i = 1; i < time.length(); i++) {
        if (time[i] == ':') {
            display.setValue(165, x - 1, y);
            display.setValue(165, x - 1, y + 1);
        }
        else {
            display.drawDigit(time[i] - '0', x, y);
            x += 4;
        }
    }

    if (isTicking) {
        display.print("Stop", 3, 3);
        display.print("Pause", 12, 3);
        options = "bSP";
    }
    else if (isPause) {
        display.print("Stop", 3, 3);
        display.print("Play", 12, 3);
        options = "bSp";
    }
    else {
        display.print("Play", 8, 3);
        options = "bhmsp";
    }

    x = 0;
    y = 1;

    switch (options[selected]) {
        case 'b':
            display.print(">", 0, 0);
            break;
        case 'h':
            if (isShow) {
                display.setValue(time[0], x, y + 1);
                display.drawDigit(time[1] - '0', x + 1, y);
            } else {
                display.setValue(' ', x, y + 1);
                display.drawDigit(10, x + 1, y);
            }
            break;
        case 'm':
            if (isShow) {
                display.drawDigit(time[3] - '0', x + 5, y);
                display.drawDigit(time[4] - '0', x + 9, y);
            } else {
                display.drawDigit(10, x + 5, y);
                display.drawDigit(10, x + 9, y);
            }
            break;
        case 's':
            if (isShow) {
                display.drawDigit(time[6] - '0', x + 13, y);
                display.drawDigit(time[7] - '0', x + 17, y);
            } else {
                display.drawDigit(10, x + 13, y);
                display.drawDigit(10, x + 17, y);
            }
            break;
        case 'p':
            display.setValue('>', isPause ? 11 : 7, 3);
            break;
        case 'S':
            display.setValue('>', 2, 3);
            break;
        case 'P':
            display.setValue('>', 11, 3);
            break;
    }

    display.show();
}

void showStopwatch(String time, uint8_t selected, bool isTicking, bool isPause) {
    display.clear();

    display.print(F("Back"), 1, 0);
    
    display.setValue(time[0], 11, 0);
    display.setValue(time[1], 13, 0);

    uint8_t x = 5;
    uint8_t y = 1;

    for (uint8_t i = 3; i < time.length(); i++) {
        if (time[i] == ':') {
            display.setValue(165, x - 1, y);
            display.setValue(165, x - 1, y + 1);
        }
        else {
            display.drawDigit(time[i] - '0', x, y);
            x += 4;
        }
    }

    if (!isTicking && !isPause) {
        display.print("Play", 1, 3);
        switch (selected) {
            case 0:
                display.setValue('>', 0, 0);
                break;
            case 1:
                display.setValue('>', 0, 3);
                break;
        }
    }
    else if (isTicking && !isPause) {
        display.print(F("Stop"), 1, 2);
        display.print(F("Pause"), 1, 3);
        switch (selected) {
            case 0:
                display.setValue('>', 0, 0);
                break;
            case 1:
                display.setValue('>', 0, 2);
                break;
            case 2:
                display.setValue('>', 0, 3);
                break;
        }
    }
    else if (isPause && !isTicking) {
        display.print(F("Stop"), 1, 2);
        display.print(F("Play"), 1, 3);
        switch (selected) {
            case 0:
                display.setValue('>', 0, 0);
                break;
            case 1:
                display.setValue('>', 0, 2);
                break;
            case 2:
                display.setValue('>', 0, 3);
                break;
        }
    }

    display.show();
}

void showTimerAlarm() {
    display.clear();

    display.print(F("Time Out"), 6, 1);
    display.print(F("Press the button"), 2, 2);
    display.print(F("to stop an alarm"), 2, 3);

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
    display.show();
}