#include <Arduino.h>
#include <EncButton.h>
#include <ClockTimer.h>
#include <ClockStopwatch.h>
#include <ClockAlarm.h>

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
bool autoBright = true;
bool brightLowOnStart = false;
void showTime(const char* time, const char* date, bool isAmFormat);
void showSensorData(int temp, int pressure);
void showDefault();
void showMenu(uint8_t menuState);
void showTimer(uint8_t selected, const char* time, bool isTicking, bool isPause, bool isShow);
void showAlarm();
void showClockAlarm(uint8_t selected) {}
void showStopwatch(const char* time, uint8_t selected, bool isTicking, bool isPause);

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

// Photoresistor functions
int getBright();

struct ClockFlags
{
    uint8_t timerActivated : 1;
    uint8_t timerPause : 1;
    uint8_t timerTicking : 1;
    uint8_t stopwatchPause : 1;
    uint8_t stopwatchTicking : 1;
};
ClockFlags clockFlags;

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
    enc.setEncType(EB_STEP4_LOW);
    enc.setFastTimeout(45);
    screenSetup();
    ledSetup();
    rtcSetup();
    sensorSetup();
    setSeaLevel(185);

    Serial.begin(9600);

    // setTime(16, 45, 0);
    // syncToClock();

    timer1.attach([]() {
        clockFlags.timerActivated = true;
    });

    // Проверка яркости чтобы не было темного экрана в темноте
    if (getBright() < 15) brightLowOnStart = true;
}

void navigateView();
void timerAlarm();

void loop() {
    enc.tick();
    screenLoop();
    ledLoop();
    timer1.tick();
    stopwatch.tick();
    
    static Timer brightTimer(150);
    if (clockFlags.timerActivated) timerAlarm();
    else navigateView();

    int currentBright = getBright();

    if (brightLowOnStart && currentBright >= 15) brightLowOnStart = false;

    if (brightTimer.ready() && autoBright && !brightLowOnStart) {
        uint8_t brightScreen = map(currentBright, 0, 500, 10, 255);
        brightScreen = constrain(brightScreen, 10, 255);
        uint8_t brightLed = map(currentBright, 0, 1010, 10, 255);
        brightLed = constrain(brightLed, 10, 255);
        setScreenBright(brightScreen);
        setLedBrightness(brightLed);
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
        if (enc.right()) viewState++;
        switch (viewState)
        {
            case 0:
                sensorView();
                break;
            case 1:
                timeView();
                break;
            case 2:
                menuView();
                break;
            case 3:
                menuView();
                break;
            case 4:
                menuView();
                break;
            case 5:
                menuView();
                break;
            default:
                defaultView();
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

    getTime(time, sizeof(time), false);
    getDate(date, sizeof(date), true);
    

    showTime(time, date, true);
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

void timerView() {
    static uint8_t selected;
    static bool isShowTime;
    static Timer isShowTimer(500);
    
    static uint8_t hours;
    static uint8_t minutes;
    static uint8_t seconds;

    if (isShowTimer.ready()) isShowTime = !isShowTime;

    if (enc.left() && selected > 0) selected--;
    if (enc.right()) {
        if ((clockFlags.timerTicking || clockFlags.timerPause) && selected < 2) selected++;
        else if (!clockFlags.timerTicking && !clockFlags.timerPause && selected < 4) selected++;
    }

    if (!clockFlags.timerPause && !clockFlags.timerTicking) {
        switch (selected) {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.turn() && enc.pressing()) {
                    isShowTimer.restart();
                    isShowTime = true;
                    hours += (((enc.dir() < 0 && hours == 0) || (enc.dir() > 0 && hours == 99)) ? 0 : enc.dir()) * ((hours >= 10 && hours <= 89 && enc.fast()) ? 10 : 1);
                } 
                break;
            case 2:
                if (enc.turn() && enc.pressing()) {
                    isShowTimer.restart();
                    isShowTime = true;
                    minutes += (((enc.dir() < 0 && minutes == 0) || (enc.dir() > 0 && minutes == 59)) ? 0 : enc.dir()) * ((minutes >= 10 && minutes <= 49 && enc.fast()) ? 10 : 1);
                } 
                break;
            case 3:
                if (enc.turn() && enc.pressing()) {
                    isShowTimer.restart();
                    isShowTime = true;
                    seconds += (((enc.dir() < 0 && seconds == 0) || (enc.dir() > 0 && seconds == 59)) ? 0 : enc.dir()) * ((seconds >= 10 && seconds <= 49 && enc.fast()) ? 10 : 1);
                } 
                break;
            case 4:
                if (enc.click()) {
                    if (hours == 0 && minutes == 0 && seconds == 0) return;

                    timer1.setTime(hours, minutes, seconds);
                    timer1.play();

                    clockFlags.timerTicking = true;
                    selected = 2;
                }
                break;
        }
    }
    else if (clockFlags.timerPause && !clockFlags.timerTicking) {
        switch (selected)
        {
            case 0: // Button Back
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    timer1.stop();
                    timer1.clear();
                    clockFlags.timerTicking = false;
                    clockFlags.timerPause = false;
                    selected = 4;
                }
                break;
            case 2:
                if (enc.click()) {
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
    else if (clockFlags.timerTicking && !clockFlags.timerPause) {
        switch (selected)
        {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    timer1.stop();
                    timer1.clear();
                    clockFlags.timerTicking = false;
                    selected = 4;
                }
                break;
            case 2:
                if (enc.click()) {
                    timer1.stop();
                    clockFlags.timerPause = true;
                    clockFlags.timerTicking = false;
                }
                break;
        }
        hours = timer1.getHours();
        minutes = timer1.getMinutes();
        seconds = timer1.getSeconds();
    }

    char time[9] = "";

    snprintf(time, 9, "%02d:%02d:%02d", hours, minutes, seconds);

    showTimer(selected, time, clockFlags.timerTicking, clockFlags.timerPause, isShowTime);
}

bool inClockAlarmSetting = false;

void clockAlarmView(ClockAlarm& clockAlarm) {
    
}

void alarmView() {
    static uint8_t selected;

    if (enc.left() && selected > 0) selected--;
    else if (enc.right() && selected < 4) selected++;

    switch (selected) {
        case 0:
            if (enc.click()) isMenu = false;
            break;
        case 1 || 2 || 3 || 4:
            if (enc.click()) {
                inClockAlarmSetting = true;
            }
            break;
    }

    if (inClockAlarmSetting) {
        switch (selected)
        {
            case 1:
                clockAlarmView(clockAlarm1);
                break;
            case 2:
                clockAlarmView(clockAlarm2);
                break;
            case 3:
                clockAlarmView(clockAlarm3);
                break;
            case 4:
                clockAlarmView(clockAlarm4);
                break;
        }
    }
    else {
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

bool notAdded = true;
void menuView() {
    
    if (isMenu) {
        switch (menuState)
        {
            case 0: // Timer
                timerView();
                break;
            case 1: // Alarm
                alarmView();
                break;
            case 2: // Stopwatch
                stopwatchView();
                break;
            // case 3:
            //     settingsView();
            //     break;
        }
    }
    else {
        uint8_t optionView = viewState - 2;
    
        if (notAdded) {
            notAdded = false;
        }
    
        if (enc.click()) {
            isMenu = true;
            menuState = optionView;
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

    autoBright = false;
    setScreenBright(255);
    showAlarm();
    setLedBrightness(255);
    setLedRGB(255, 0, 0);
    setBlinkingSettings(4, 75, 1000);
    setLedMode(BLINK_MODE);
    alarm();

    if (enc.click()) {
        autoBright = true;
        clockFlags.timerActivated = false;
    }
}

void defaultView() {
    showDefault();
}