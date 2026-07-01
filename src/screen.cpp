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

    uint8_t hours = (time[0] - '0') * 10 + (time[1] - '0');
    uint8_t minutes = (time[3] - '0') * 10 + (time[4] - '0');
    
    char ap[] = "MM";
    
    if (isAmFormat) {
        ap[0] = (hours > 12) ? 'P' : 'A';
        if (hours > 12) hours -= 12;
    }
    
    char newTime[6] = "";
    snprintf(newTime, 6, "%02d:%02d", hours, minutes);

    for (uint8_t i = 0; i < strlen(newTime); i++) {
        if (newTime[i] == ':') {
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
            display.drawDigit(newTime[i] - '0', x, y);
            x += 4;
        }
    }

    uint8_t dateX = 10 - (strlen(date) / 2);

    display.print(date, dateX, 3);
    if (isAmFormat) display.print(ap, 18, 2);
    display.show();
}

void showSensorData(int temp, int pressure) {
    display.clear();
    
    char stringTemp[5] = "";
    snprintf(stringTemp, 5, "%02d", temp);
    stringTemp[3] = stringTemp[2];
    stringTemp[2] = '.';

    uint8_t x = 0;
    uint8_t y = 0;

    for (unsigned int i = 0; i < strlen(stringTemp); i++) {
        if (stringTemp[i] == '.') {
            display.setValue('.', x - 1, y + 1);
        }
        else {
            display.drawDigit(stringTemp[i] - '0', x, y);
            x += 4;
        }
    }
    display.setValue(111, x - 1, y);
    display.setValue('C', x, y);

    char stringPressure[4] = "";
    snprintf(stringPressure, 4, "%02d", pressure);

    x = 7;
    y = 2;

    for (unsigned int i = 0; i < strlen(stringPressure); i++) {
        display.drawDigit(stringPressure[i] - '0', x, y);
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

void showTimer(uint8_t selected, const char* time, bool isTicking, bool isPause, bool isShow) {
    char options[10] = "bhmsp";

    display.clear();
    display.print(F("Back"), 1, 0);

    display.setValue(time[0], 0, 2);

    uint8_t x = 1;
    uint8_t y = 1;
    for (uint8_t i = 1; i < strlen(time); i++) {
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
        strcpy(options, "bSP");
    }
    else if (isPause) {
        display.print("Stop", 3, 3);
        display.print("Play", 12, 3);
        strcpy(options, "bSp");
    }
    else {
        display.print("Play", 8, 3);
        strcpy(options, "bhmsp");
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

void showStopwatch(const char* time, uint8_t selected, bool isTicking, bool isPause) {
    display.clear();

    display.print(F("Back"), 1, 0);
    
    display.setValue(time[0], 11, 0);
    display.setValue(time[1], 13, 0);

    uint8_t x = 5;
    uint8_t y = 1;

    for (uint8_t i = 3; i < strlen(time); i++) {
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

void showAlarm() {
    display.clear();

    display.print(F("Time Out"), 6, 1);
    display.print(F("Press the button"), 2, 2);
    display.print(F("to stop an alarm"), 2, 3);

    display.show();
}

uint8_t targetValue = 50;

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