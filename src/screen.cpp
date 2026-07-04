#include <Arduino.h>
#include <Display.h>
#include <Timer.h>

Display display(0x27, 3);

void screenSetup() {
    display.init();
}

void showTime(const char* time, const char* date, bool isAmFormat, bool showSeconds) {
    static Timer dotsTimer(500);
    static bool dots;

    if (dotsTimer.ready()) dots = !dots;

    display.clear();

    uint8_t hours = (time[0] - '0') * 10 + (time[1] - '0');
    uint8_t minutes = (time[3] - '0') * 10 + (time[4] - '0');

    if (isAmFormat) {
        char ap[3] = "AM";
        if (hours > 12) {
            hours -= 12;
            ap[0] = 'P';
        }
        display.print(ap, 18, 2);
    }
    // uint8_t seconds = (time[6] - '0') * 10 + (time[7] - '0');

    display.drawDigit(hours / 10, 3, 1);
    display.drawDigit(hours % 10, 7, 1);

    if (dots) {
        display.setValue(165, 10, 1);
        display.setValue(165, 10, 2);
    }

    display.drawDigit(minutes / 10, 11, 1);
    display.drawDigit(minutes % 10, 15, 1);

    if (showSeconds) {
        display.setValue(time[6], 18, 1);
        display.setValue(time[7], 19, 1);
    }

    uint8_t dateX = 10 - (strlen(date) / 2);
    display.print(date, dateX, 3);

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

void showTimer(uint8_t selected, uint8_t hours, uint8_t minutes, uint8_t seconds, bool isTicking, bool isPause, bool isShow) {
    display.clear();

    display.print(F("Back"), 1, 0);

    display.setValue(hours / 10 + '0', 0, 2);
    display.drawDigit(hours % 10, 1, 1);
    display.setValue(165, 4, 1);
    display.setValue(165, 4, 2);
    display.drawDigit(minutes / 10, 5, 1);
    display.drawDigit(minutes % 10, 9, 1);
    display.setValue(165, 12, 1);
    display.setValue(165, 12, 2);
    display.drawDigit(seconds / 10, 13, 1);
    display.drawDigit(seconds % 10, 17, 1);

    if (!isTicking && !isPause) {
        display.print(F("Play"), 8, 3);
        switch (selected)
        {
            case 0:
                display.setValue('>', 0, 0);
                break;
            case 1:
                if (!isShow) {
                    display.setValue(' ', 0, 2);
                    display.drawDigit(10, 1, 1);
                }
                break;
            case 2:
                if (!isShow) {
                    display.drawDigit(10, 5, 1);
                    display.drawDigit(10, 9, 1);
                }
                break;
            case 3:
                if (!isShow) {
                    display.drawDigit(10, 13, 1);
                    display.drawDigit(10, 17, 1);
                }
                break;
            case 4:
                display.setValue('>', 7, 3);
                break;
        }
    } else if (!isTicking && isPause) {
        display.print(F("Stop"), 3, 3);
        display.print(F("Play"), 12, 3);

        switch (selected){
            case 0:
                display.setValue('>', 0, 0);
                break;
            case 1:
                display.setValue('>', 2, 3);
                break;
            case 2:    
                display.setValue('>', 11, 3);
                break;
        }
    } else if (isTicking && !isPause) {
        display.print(F("Stop"), 3, 3);
        display.print(F("Pause"), 12, 3);

        switch (selected) {
            case 0:
                display.setValue('>', 0, 0);
                break;
            case 1:
                display.setValue('>', 2, 3);
                break;
            case 2:
                display.setValue('>', 11, 3);
                break;
        }
    }

    display.show();
}

void showClockAlarm(uint8_t selected) {
    display.clear();

    display.print(F("Back"), 1, 0);
    display.print(F("Clock 1"), 13, 0);
    display.print(F("Clock 2"), 13, 1);
    display.print(F("Clock 3"), 13, 2);
    display.print(F("Clock 4"), 13, 3);

    switch (selected) {
        case 0:
            display.setValue('>', 0, 0);
            break;
        case 1:
            display.setValue('>', 12, 0);
            break;
        case 2:
            display.setValue('>', 12, 1);
            break;
        case 3:
            display.setValue('>', 12, 2);
            break;
        case 4:
            display.setValue('>', 12, 3);
            break;
    }

    display.show();
}

void showClockAlarmSettings(uint8_t selected, uint8_t hour, bool isAmPmFormat, uint8_t minute, bool isTurnedOn, uint8_t isShow) {
    display.clear();

    display.print(F("Back"), 1, 0);

    char ap[3] = "AM";

    if (isAmPmFormat && hour > 12) {
        hour -= 12;
        ap[0] = 'P';
    }

    char stringHours[3] = "";
    char stringMinutes[3] = "";
    snprintf(stringHours, 3, "%02d", hour);
    snprintf(stringMinutes, 3, "%02d", minute);

    display.drawDigit(stringHours[0] - '0', 3, 1);
    display.drawDigit(stringHours[1] - '0', 7, 1);
    display.setValue(165, 10, 1);
    display.setValue(165, 10, 2);
    display.drawDigit(stringMinutes[0] - '0', 11, 1);
    display.drawDigit(stringMinutes[1] - '0', 15, 1);
    if (isAmPmFormat) {
        display.print(ap, 18, 2);
    }

    display.print(F("Turned: "), 1, 3);
    display.print((isTurnedOn) ? F("On") : F("Off"), 9, 3);

    display.print(F("Save"), 15, 3);


    switch (selected)
    {
        case 0:
            display.setValue('>', 0, 0);
            break;
        case 1:
            if (!isShow) {
                display.drawDigit(10, 3, 1);
                display.drawDigit(10, 7, 1);
            }
            break;
        case 2:
            if (!isShow) {
                display.drawDigit(10, 11, 1);
                display.drawDigit(10, 15, 1);
            }
            break;
        case 3:
            display.setValue('>', 0, 3);
            break;
        case 4:
            display.setValue('>', 14, 3);
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

void showSettings(uint8_t selected, bool amPmFormat, bool autoBright, uint8_t brightness, bool showSeconds, bool isParameterChanging, uint8_t parameterSelected, uint8_t hours, uint8_t minutes, uint8_t day, uint8_t month, uint8_t year) {
    display.clear();

    if (selected >= 0 && selected <= 3) {
        display.print(F("Back"), 1, 0);
        display.setValue('v', 19, 0);

        display.print(F("AM/PM: "), 1, 1);
        display.print((amPmFormat) ? F("On") : F("Off"), 8, 1);

        display.print(F("Auto Bright: "), 1, 2);
        display.print((autoBright) ? F("On"): F("Off"), 14, 2);

        display.print(F("Bright: "), 1, 3);
        if (autoBright) display.print(F("Unavailable"), 9, 3);
        else {
            char stringBright[5];
            
            if (brightness < 10) {
                stringBright[0] = brightness + '0';
                stringBright[1] = '%';
                stringBright[2] = '\0';
            }
            else if (brightness >= 10 && brightness < 100) {
                stringBright[0] = brightness / 10 + '0';
                stringBright[1] = brightness % 10 + '0';
                stringBright[2] = '%';
                stringBright[3] = '\0';
            } else {
                stringBright[0] = '1';
                stringBright[1] = '0';
                stringBright[2] = '0';
                stringBright[3] = '%';
                stringBright[4] = '\0';
            }
            display.print(stringBright, 9, 3);
        }
        if (isParameterChanging) {
            if (selected == 3) {
                display.setValue('>', 8, 3);
            }
        }
        else display.setValue('>', 0, selected);
    }
    else if (selected > 3 && selected <= 7) {
        display.setValue(94, 19, 0);

        display.print(F("Time: "), 1, 0);
        if (amPmFormat) {
            char time[9];
            char ap[3] = "AM";
            if (hours > 12) {
                ap[0] = 'P';
                hours -= 12;
            }
            snprintf(time, 9, "%02d %02d %s", hours, minutes, ap);
            display.print(time, 7, 0);
        } else {
            char time[6];
            snprintf(time, 6, "%02d %02d", hours, minutes);
            display.print(time, 7, 0);
        }         

        display.print(F("Date: "), 1, 1);
        char date[11];
        snprintf(date, 11, "%02d %02d 20%02d", day, month, year);
        display.print(date, 7, 1);

        display.print(F("Show seconds: "), 1, 2);
        display.print((showSeconds) ? F("On") : F("Off"), 15, 2);

        display.print(F("Reset"), 1, 3);

        if (isParameterChanging) {
            if (selected == 4) {
                if (parameterSelected == 0) display.setValue('>', 6, 0);
                else if (parameterSelected == 1) display.setValue('>', 9, 0);
            } else if (selected == 5) {
                if (parameterSelected == 0) display.setValue('>', 6, 1);
                else if (parameterSelected == 1) display.setValue('>', 9, 1);
                else if (parameterSelected == 2) display.setValue('>', 12, 1);
            } else if (selected == 7) {
                display.clear();

                display.print(F("Do you want to reset"), 0, 1);
                display.print(F("settings to default?"), 0, 2);

                display.print(F("No"), 1, 3);
                display.print(F("Yes"), 16, 3);

                switch (parameterSelected)
                {
                    case 0:
                        display.setValue('>', 0, 3);
                        break;
                    case 1:
                        display.setValue('>', 15, 3);
                        break;
                }
            }
        } else {
            display.setValue('>', 0, selected - 4);
        }
    }

    display.show();
}

void showResetCompleted() {
    display.clear();

    display.print(F("Reset completed!"), 2, 1);
    display.print(F("Please, reboot the"), 1, 2);
    display.print(F("device"), 8, 3);

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