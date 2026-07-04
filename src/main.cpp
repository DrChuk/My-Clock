#include <Arduino.h>
#include <EncButton.h>
#include <ClockTimer.h>
#include <ClockStopwatch.h>
#include <ClockAlarm.h>
#include <Settings.h>

// 0x27 - LCD, 0x57 - ?, 0x68 - DS3231 (RTC), 0x77 - BMP280 (Temperature Sensor)

// Encoder
#define S1 2
#define S2 4
#define KEY 7
EncButton enc(S1, S2, KEY);

// Buzzer functions
// Sound Effects
void alarm();

// Quiet Mode
void setQuietMode(bool value);
void switchQuietMode();
bool getQuietMode();

// Sensor functions
void sensorSetup();
void setSeaLevel(float seaLevel);
int getTemperature();
int getPressure();

// Photoresistor
int getBright();

// Display functions
void screenSetup();
void screenLoop();
void setScreenBright(uint8_t value);
bool brightLowOnStart = false;
void showTime(const char* time, const char* date, bool isAmFormat, bool showSeconds);
void showSensorData(int temp, int pressure);
void showDefault();
void showMenu(uint8_t menuState);
void showTimer(uint8_t selected, uint8_t hours, uint8_t minutes, uint8_t seconds, bool isTicking, bool isPause, bool isShow);
void showAlarm();
void showClockAlarm(uint8_t selected);
void showClockAlarmSettings(uint8_t selected, uint8_t hour, bool isAmPmFormat, uint8_t minute, bool isTurnedOn, uint8_t isShow);
void showStopwatch(const char* time, uint8_t selected, bool isTicking, bool isPause);
void showSettings(uint8_t selected, bool amPmFormat, bool autoBright, uint8_t brightness, bool showSeconds, bool isParameterChanging, uint8_t parameterSelected, uint8_t hours, uint8_t minutes, uint8_t day, uint8_t month, uint8_t year);
void showResetCompleted();

// RGB functions
#define GLOW_MODE 0
#define BLINK_MODE 1
#define BREATHE_MODE 2

void ledSetup();
void ledLoop();
void setLedRGB(uint8_t red, uint8_t green, uint8_t blue);
void setLedHue(uint8_t hue);
void setLedBrightness(uint8_t bright);
void setLedMode(uint8_t modeNumber);
void setBreatheDuration(uint8_t value);
void setBlinkingSettings(uint8_t countsN, uint32_t durationN, uint32_t cdN);

// RTC functions
void rtcSetup();
void getTime(char *buffer, uint8_t bufferSize, bool sec);
void getDate(char* buffer, uint8_t bufferSize, bool withWeekday);
void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void setDate(uint8_t day, uint8_t month, uint8_t year);
void syncToClock();
void syncFromClock();

// EEPROM functions
void firstOn(Settings& settings);
void settingsToDefault();
void saveSettings(Settings& settings);

// Photoresistor functions
int getBright();

struct ClockFlags {
    uint8_t timerActivated : 1;
    uint8_t alarmActivated : 1;
    uint8_t timerPause : 1;
    uint8_t timerTicking : 1;
    uint8_t stopwatchPause : 1;
    uint8_t stopwatchTicking : 1;
};

ClockFlags clockFlags;
Settings settings;

// Clock things
// Timer
ClockTimer timer1;
// Alarms
ClockAlarm clockAlarm1;
ClockAlarm clockAlarm2;
ClockAlarm clockAlarm3;
ClockAlarm clockAlarm4;
// Stopwatch
Stopwatch stopwatch;

void setup() {
    firstOn(settings);

    enc.setEncType(EB_STEP4_LOW);
    enc.setFastTimeout(45);
    // enc.setDebTimeout(10);
    screenSetup();
    ledSetup();
    rtcSetup();
    sensorSetup();
    setSeaLevel(185);

    Serial.begin(9600);

    timer1.attach([]() {
        clockFlags.timerActivated = true;
    });

    clockAlarm1.setTime(settings.alarms.clock1.hour, settings.alarms.clock1.minute);
    clockAlarm1.setTurned(settings.alarms.clock1.isTurnedOn);
    clockAlarm2.setTime(settings.alarms.clock2.hour, settings.alarms.clock2.minute);
    clockAlarm2.setTurned(settings.alarms.clock2.isTurnedOn);
    clockAlarm3.setTime(settings.alarms.clock3.hour, settings.alarms.clock3.minute);
    clockAlarm3.setTurned(settings.alarms.clock3.isTurnedOn);
    clockAlarm4.setTime(settings.alarms.clock4.hour, settings.alarms.clock4.minute);
    clockAlarm4.setTurned(settings.alarms.clock4.isTurnedOn);

    clockAlarm1.attach([](){clockFlags.alarmActivated = true;});
    clockAlarm2.attach([](){clockFlags.alarmActivated = true;});
    clockAlarm3.attach([](){clockFlags.alarmActivated = true;});
    clockAlarm4.attach([](){clockFlags.alarmActivated = true;});

    // Checking brightness on the start, if device in a dark place, then set min bright
    if (getBright() < 15) brightLowOnStart = true;
}

void navigateView();
void timerAlarm();
void clockAlarm();

void loop() {
    enc.tick();
    screenLoop();
    ledLoop();
    timer1.tick();
    stopwatch.tick();

    char time[6] = "";
    getTime(time, 6, false);
    uint8_t hours = (time[0] - '0') * 10 + (time[1] - '0');
    uint8_t minutes = (time[3] - '0') * 10 + (time[4] - '0');

    clockAlarm1.tick(hours, minutes);
    clockAlarm2.tick(hours, minutes);
    clockAlarm3.tick(hours, minutes);
    clockAlarm4.tick(hours, minutes);
    
    static Timer brightTimer(150);
    if (clockFlags.timerActivated) timerAlarm();
    else if (clockFlags.alarmActivated) clockAlarm();
    else navigateView();

    int currentBright = getBright();

    if (settings.autoBright && brightLowOnStart && currentBright >= 15) brightLowOnStart = false;

    if (brightTimer.ready()) {
        if (settings.autoBright && !brightLowOnStart && !clockFlags.timerActivated && !clockFlags.alarmActivated) {
            uint8_t brightScreen = map(currentBright, 0, 500, 10, 255);
            brightScreen = constrain(brightScreen, 10, 255);
            uint8_t brightLed = map(currentBright, 0, 1010, 10, 255);
            brightLed = constrain(brightLed, 10, 255);
            setScreenBright(brightScreen);
            setLedBrightness(brightLed);
        }
        else if (!settings.autoBright && !clockFlags.alarmActivated && !clockFlags.timerActivated) {
            uint8_t brightScreen = constrain(map(settings.brightness, 0, 100, 10, 255), 10, 255);
            uint8_t brightLed = constrain(map(settings.brightness, 0, 100, 10, 50), 10, 50);

            setScreenBright(brightScreen);
            setLedBrightness(brightLed);
        }
    }
}

// All views
void timeView();
void sensorView();
void menuView();
void timerView();
void defaultView();

uint8_t viewState = 1; // 1 - Time and Date; 0 - Temperature and Pressure; 2 - Menu
uint8_t menuState = 0; // 0 - Timer; 1 - Alarm; 2 - Stopwatch; 3 - Settings
bool isMenu;

void navigateView() {
    static Timer clockUpdateTimer(500);

    if (!isMenu) {
        if (enc.left() && viewState > 0) viewState--;
        if (enc.right() && viewState < 5) viewState++;
        switch (viewState)
        {
            case 0:
                sensorView();
                break;
            case 1:
                timeView();
                break;
            case 2:
            case 3:
            case 4:
            case 5:
                menuView();
                break;
        }
    }
    else {
        menuView();
    }
}

void timeView() {
    char time[9] = "";
    char date[20] = "";
    syncFromClock();

    getTime(time, sizeof(time), true);
    getDate(date, sizeof(date), true);

    showTime(time, date, settings.amPmFormat, settings.showSeconds);
    setLedRGB(255, 255, 255);
    setLedMode(GLOW_MODE);
}

void sensorView() {
    int pressure = getPressure() * 0.7506;

    showSensorData(getTemperature(), pressure);

    if (pressure < 735) {
        setLedRGB(0, 0, 255);
        setLedMode(BLINK_MODE);
        setBlinkingSettings(1, 500, 500);
    }
    else if (pressure >= 735 && pressure < 754) {
        setLedRGB(0, 0, 255);
        setLedMode(GLOW_MODE);
    }
    else if (pressure >= 754 && pressure < 769) {
        setLedRGB(0, 255, 0);
        setLedMode(GLOW_MODE);
    }
    else if (pressure >= 769 && pressure < 780) {
        setLedRGB(175, 255, 0);
        setLedMode(GLOW_MODE);
    }
    else if (pressure >= 780) {
        setLedRGB(255, 0, 0);
        setLedMode(BLINK_MODE);
        setBlinkingSettings(1, 500, 500);
    }
}

bool timerChangeParametrs = true;

void timerView() {
    static uint8_t selected;
    static bool isParameterChanging;
    static bool isShowingClock;
    static Timer isShowingClockTimer(500);

    static uint8_t hours;
    static uint8_t minutes;
    static uint8_t seconds;

    if (timerChangeParametrs) {
        hours = 0;
        minutes = 0;
        seconds = 0;
        timerChangeParametrs = false;
    }

    if (isShowingClockTimer.ready()) isShowingClock = !isShowingClock;

    bool left = enc.left();
    bool right = enc.right();
    bool click = enc.click();

    if (isParameterChanging) {
        switch (selected) {
            case 1:
                if (left && hours > 0) hours--;
                else if (right && hours < 99) hours++;
                if (click) {
                    isParameterChanging = false;
                    isShowingClockTimer.enable();
                }
                break;
            case 2:
                if (left && minutes > 0) minutes--;
                else if (right && minutes < 59) minutes++;
                if (click) {
                    isParameterChanging = false;
                    isShowingClockTimer.enable();
                }
                break;
            case 3:
                if (left && seconds > 0) seconds--;
                else if (right && seconds < 59) seconds++;
                if (click) {
                    isParameterChanging = false;
                    isShowingClockTimer.enable();
                }
                break;
        }
    } else {
        if (!clockFlags.timerTicking && !clockFlags.timerPause) {
            if (left && selected > 0) selected--;
            else if (right && selected < 4) selected++;
            switch (selected)
            {
                case 0:
                    if (click) isMenu = false;
                    break;
                case 1:
                case 2:
                case 3:
                    if (click) {
                        isParameterChanging = true;
                        isShowingClockTimer.disable();
                        isShowingClock = true;
                    }
                    break;
                case 4:
                    if (click) {
                        if (hours > 0 || minutes > 0 || seconds > 0) {
                            clockFlags.timerTicking = true;
                            selected = 2;
                            timer1.setTime(hours, minutes, seconds);
                            timer1.play();
                        }
                    }
                    break;
            }
        } else if (!clockFlags.timerPause && clockFlags.timerTicking) {
            if (left && selected > 0) selected--;
            else if (right && selected < 2) selected++;
            switch (selected)
            {
                case 0:
                    if (click) isMenu = false;
                    break;
                case 1:
                    if (click) {
                        timer1.stop();
                        timer1.clear();
                        selected = 4;
                        clockFlags.timerTicking = false;
                    }
                    break;
                case 2:
                    if (click) {
                        timer1.stop();
                        clockFlags.timerPause = true;
                        clockFlags.timerTicking = false;
                    }
                    break;
            }
            
            hours = timer1.getHours();
            minutes = timer1.getMinutes();
            seconds = timer1.getSeconds();
        } else if (!clockFlags.timerTicking && clockFlags.timerPause) {
            if (left && selected > 0) selected--;
            else if (right && selected < 2) selected++;
            switch (selected)
            {
                case 0:
                    if (click) isMenu = false;
                    break;
                case 1:
                    if (click) {
                        timer1.stop();
                        timer1.clear();
                        selected = 4;
                        clockFlags.timerPause = false;
                    }
                    break;
                case 2:
                    if (click) {
                        timer1.play();
                        clockFlags.timerPause = false;
                        clockFlags.timerTicking = true;
                    }
                    break;
            }

            hours = timer1.getHours();
            minutes = timer1.getMinutes();
            seconds = timer1.getSeconds();
        }
    }

    showTimer(selected, hours, minutes, seconds, clockFlags.timerTicking, clockFlags.timerPause, isShowingClock);
}

bool inClockAlarmSetting = false;
bool newParameters = false;

void clockAlarmSettingsView(uint8_t alarmSelected) {
    static uint8_t selected;
    static bool isParameterChanging;

    static uint8_t hour;
    static uint8_t minute;
    static bool isTurnedOn;

    ClockAlarm* clockAlarm;
    switch (alarmSelected) {
        case 1:
            clockAlarm = &clockAlarm1;
            break;
        case 2:
            clockAlarm = &clockAlarm2;
            break;
        case 3:
            clockAlarm = &clockAlarm3;
            break;
        case 4:
            clockAlarm = &clockAlarm4;
            break;
    }

    if (newParameters) {
        hour = clockAlarm->getHour();
        minute = clockAlarm->getMinute();
        isTurnedOn = clockAlarm->getTurned();
        newParameters = false;
    }

    static Timer showTimer(500);
    static bool isShowSelected;

    if (showTimer.ready()) isShowSelected = !isShowSelected;


    if (isParameterChanging) {
        switch (selected) {
            case 1:
                if (enc.left() && hour > 0) hour--;
                else if (enc.right() && hour < 23) hour++;
                if (enc.click()) {
                    isParameterChanging = false;
                    showTimer.enable();
                }
                break;
            case 2:
                if (enc.left() && minute > 0) minute -= 5;
                else if (enc.right() && minute < 55) minute += 5;
                if (enc.click()) {
                    isParameterChanging = false;
                    showTimer.enable();
                }
                break;
        }
    } else {
        if (enc.left() && selected > 0) selected--;
        else if (enc.right() && selected < 4) selected++;

        switch (selected) {
            case 0:
                if (enc.click()) {
                    inClockAlarmSetting = false;
                    enc.clear();
                }
                break;
            case 1:
                // if (enc.turnH()) {
                //     showTimer.restart();
                //     isShowSelected = true;
                //     hour += ((enc.dir() > 0 && hour < 23) || (enc.dir() < 0 && hour > 0)) ? enc.dir() : 0;
                // }
            case 2:
                // if (enc.turnH()) {
                    //     showTimer.restart();
                    //     isShowSelected = true;
                    //     minute += ((enc.dir() > 0 && minute < 59) || (enc.dir() < 0 && minute > 0)) ? enc.dir() : 0;
                    // }
                    if (enc.click()) {
                        isShowSelected = true;
                        showTimer.disable();
                        isParameterChanging = true;
                    }
                break;
            case 3:
                if (enc.click()) isTurnedOn = !isTurnedOn;
                break;
            case 4:
                if (enc.click()) {
                    clockAlarm->setTurned(isTurnedOn);
                    clockAlarm->setTime(hour, minute);
    
                    switch (alarmSelected) {
                        case 1:
                            settings.alarms.clock1.hour = clockAlarm->getHour();
                            settings.alarms.clock1.minute = clockAlarm->getMinute();
                            settings.alarms.clock1.isTurnedOn = clockAlarm->getTurned();
                            break;
                        case 2:
                            settings.alarms.clock2.hour = clockAlarm->getHour();
                            settings.alarms.clock2.minute = clockAlarm->getMinute();
                            settings.alarms.clock2.isTurnedOn = clockAlarm->getTurned();
                            break;
                        case 3:
                            settings.alarms.clock3.hour = clockAlarm->getHour();
                            settings.alarms.clock3.minute = clockAlarm->getMinute();
                            settings.alarms.clock3.isTurnedOn = clockAlarm->getTurned();
                            break;
                        case 4:
                            settings.alarms.clock4.hour = clockAlarm->getHour();
                            settings.alarms.clock4.minute = clockAlarm->getMinute();
                            settings.alarms.clock4.isTurnedOn = clockAlarm->getTurned();
                            break;
                    }
    
                    saveSettings(settings);
                }
                break;
        }
    }
    showClockAlarmSettings(selected, hour, settings.amPmFormat, minute, isTurnedOn, isShowSelected);
}

void clockAlarmView() {
    static uint8_t selected;

    if (inClockAlarmSetting) {
        clockAlarmSettingsView(selected);
    }
    else {
        if (enc.left() && selected > 0) selected--;
        else if (enc.right() && selected < 4) selected++;

        switch (selected) {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                if (enc.click()) {
                    inClockAlarmSetting = true;
                    newParameters = true;
                    enc.clear();
                }
                break;
        }

        showClockAlarm(selected);
    }
}

void stopwatchView() {
    static uint8_t selected;

    if (!clockFlags.stopwatchPause && !clockFlags.stopwatchTicking) {
        if (enc.right() && selected < 1) selected++;
        if (enc.left() && selected > 0) selected--;

        switch (selected) {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    stopwatch.play();
                    clockFlags.stopwatchTicking = true;
                    selected = 2;
                }
                break;
        }
    }
    else if (clockFlags.stopwatchPause && !clockFlags.stopwatchTicking) {
        if (enc.right() && selected < 2) selected++;
        if (enc.left() && selected > 0) selected--;

        switch (selected) {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    stopwatch.clear();
                    selected = 1;
                    clockFlags.stopwatchPause = false;
                }
                break;
            case 2:
                if (enc.click()) {
                    stopwatch.play();
                    clockFlags.stopwatchTicking = true;
                    clockFlags.stopwatchPause = false;
                }
                break;
        }
    }
    else if (clockFlags.stopwatchTicking && !clockFlags.stopwatchPause) {
        if (enc.right() && selected < 2) selected++;
        if (enc.left() && selected > 0) selected--;

        switch (selected) {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    stopwatch.stop();
                    stopwatch.clear();
                    selected = 1;
                    clockFlags.stopwatchTicking = false;
                }
                break;
            case 2:
                if (enc.click()) {
                    stopwatch.stop();
                    clockFlags.stopwatchPause = true;
                    clockFlags.stopwatchTicking = false;
                }
                break;
        }
    }

    char time[9] = "";
    char stringHours[3] = "";
    char stringMinutes[3] = "";
    char stringSeconds[3] = "";

    uint8_t hours = stopwatch.getHours();
    uint8_t minutes = stopwatch.getMinutes();
    uint8_t seconds = stopwatch.getSeconds();

    itoa(hours, stringHours, 3);
    itoa(minutes, stringMinutes, 3);
    itoa(seconds, stringSeconds, 3);

    snprintf(time, 9, "%02d:%02d:%02d", hours, minutes, seconds);

    showStopwatch(time, selected, clockFlags.stopwatchTicking, clockFlags.stopwatchPause);
}

bool newSettings = true;
void settingsView() {
    static uint8_t selected;
    static uint8_t parameterSelected;
    static bool isParameterChanging;

    static uint8_t hours;
    static uint8_t minutes;
    static uint8_t seconds;
    static uint8_t day;
    static uint8_t month;
    static uint8_t year;

    if (newSettings) {
        char time[8];
        char date[11];
    
        getTime(time, 6, true);
        getDate(date, 11, false);

        hours = (time[0] - '0') * 10 + (time[1] - '0');
        minutes = (time[3] - '0') * 10 + (time[4] - '0');
        seconds = (time[6] - '0') * 10 + (time[7] - '0');

        day = (date[0] - '0') * 10 + (date[1] - '0');
        month = (date[3] - '0') * 10 + (date[4] - '0');
        year = (date[8] - '0') * 10 + (date[9] - '0');

        newSettings = false;
    }

    if (isParameterChanging) {
        if (selected == 3) {
            if (enc.left() && settings.brightness > 0) settings.brightness -= 5;
            else if (enc.right() && settings.brightness < 100) settings.brightness += 5;
    
            if (enc.click()) {
                isParameterChanging = false;
                saveSettings(settings);
            }
        } else if (selected == 4) {
            switch (parameterSelected)
            {
                case 0:
                    if (enc.left() && hours > 0) hours--;
                    else if (enc.right() && hours < 23) hours++;
                    if (enc.click()) parameterSelected++;
                    break;
                case 1:
                    if (enc.left() && minutes > 0) minutes--;
                    else if (enc.right() && minutes < 59) minutes++;
                    if (enc.click()) parameterSelected++;
                    break;
                case 2:
                    isParameterChanging = false;
                    parameterSelected = 0;
                    setTime(hours, minutes, 0);
                    setDate(day, month, year);
                    syncToClock();
                    break;
            }
        } else if (selected == 5) {
            switch (parameterSelected) {
                case 0:
                    if (enc.left() && day > 1) day--;
                    else if (enc.right() && day < 31) day++;
                    if (enc.click()) parameterSelected++;
                    break;
                case 1:
                    if (enc.left() && month > 1) month--;
                    else if (enc.right() && month < 12) month++;
                    if (enc.click()) parameterSelected++;
                    break;
                case 2:
                    if (enc.left() && year > 26) year--;
                    else if (enc.right() && year < 99) year++;
                    if (enc.click()) parameterSelected++;
                    break;
                case 3:
                    isParameterChanging = false;
                    parameterSelected = 0;
                    setTime(hours, minutes, seconds);
                    setDate(day, month, year);
                    syncToClock();
                    break;
            }
        } else if (selected == 7) {
            switch (parameterSelected)
            {
                case 0:
                    if (enc.right()) parameterSelected++;
                    if (enc.click()) isParameterChanging = false;
                    break;
                case 1:
                    if (enc.left()) parameterSelected--;
                    if (enc.click()) parameterSelected++;
                    break;
                case 2:
                    setTime(0, 0, 0);
                    setDate(1, 1, 26);
                    syncToClock();
                    settingsToDefault();
                    showResetCompleted();
                    while (1) {}
            }
        }
    }
    else {
        if (enc.left() && selected > 0) selected--;
        else if (enc.right() && selected < 7) selected++;
    
        switch (selected) {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    settings.amPmFormat = !settings.amPmFormat;
                    saveSettings(settings);
                }
                break;
            case 2:
                if (enc.click()) {
                    settings.autoBright = !settings.autoBright;
                    saveSettings(settings);
                }
                break;
            case 3:
                if (enc.click() && !settings.autoBright) isParameterChanging = true;
                break;
            case 4:
            case 5:
                if (enc.click()) isParameterChanging = true;
                break;
            case 6:
                if (enc.click()) {
                    settings.showSeconds = !settings.showSeconds;
                    saveSettings(settings);
                }
                break;
            case 7:
                if (enc.click()) isParameterChanging = true;
                break;
        }
    }

    showSettings(selected, settings.amPmFormat, settings.autoBright, settings.brightness, settings.showSeconds, isParameterChanging, parameterSelected, hours, minutes, day, month, year);
}

void menuView() {
    
    if (isMenu) {
        switch (menuState)
        {
            case 0: // Timer
                timerView();
                break;
            case 1: // Alarm
                clockAlarmView();
                break;
            case 2: // Stopwatch
                stopwatchView();
                break;
            case 3:
                settingsView();
                break;
        }
    }
    else {
        uint8_t optionView = viewState - 2;
    
        if (enc.click()) {
            isMenu = true;
            menuState = optionView;

            if (optionView == 0) timerChangeParametrs = true;
            else if (optionView == 3) newSettings = true;
        }

        showMenu(optionView);
    }
    
    setLedRGB(255, 255, 255);
    setLedMode(GLOW_MODE);
}

void timerAlarm() {
    timer1.stop();
    timer1.clear();
    clockFlags.timerPause = false;
    clockFlags.timerTicking = false;

    setScreenBright(255);
    showAlarm();
    setLedBrightness(255);
    setLedRGB(255, 0, 0);
    setBlinkingSettings(4, 75, 1000);
    setLedMode(BLINK_MODE);
    alarm();

    if (enc.click()) {
        clockFlags.timerActivated = false;
    }
}

void clockAlarm() {
    setScreenBright(255);
    showAlarm();
    setLedBrightness(255);
    setLedRGB(255, 0, 0);
    setBlinkingSettings(4, 75, 1000);
    setLedMode(BLINK_MODE);
    alarm();

    if (enc.click()) {
        clockFlags.alarmActivated = false;
    }
}

void defaultView() {
    showDefault();
}