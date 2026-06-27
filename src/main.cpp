#include <Arduino.h>
#include <EncButton.h>
#include <ClockTimer.h>
#include <ClockStopwatch.h>

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
float getTemperature();
float getPressure();

// Photoresistor
int getBright();

// Display functions
void screenSetup();
void screenLoop();
void setScreenBright(uint8_t value);
bool autoBright = true;
void showTime(String& time, String& date, bool isAmFormat);
void showSensorData(float temp, float pressure);
void showDefault();
void showMenu(uint8_t menuState);
void showTimer(uint8_t selected, String time, bool isTicking, bool isPause, bool isShow);
void showTimerAlarm();
void showStopwatch(String time, uint8_t selected, bool isTicking, bool isPause);

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
void getTime(String *buffer, bool sec);
void getDate(String *buffer, bool withWeekday);
void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void setDate(uint8_t day, uint8_t month, uint8_t year);
void updateClock();

// Photoresistor functions
int getBright();

// Clock things
// Timer
ClockTimer timer1;
bool timerActivated;
bool timerPause;
bool timerTicking;
// Stopwatch
Stopwatch stopwatch;
bool stopwatchPause;
bool stopwatchTicking;

void setup() {
    enc.setEncType(EB_STEP4_LOW);
    enc.setFastTimeout(45);
    screenSetup();
    ledSetup();
    rtcSetup();
    sensorSetup();
    setSeaLevel(185);

    Serial.begin(9600);

    timer1.attach([]() {
        timerActivated = true;
    });
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
    if (timerActivated) timerAlarm();
    else navigateView();

    if (brightTimer.ready() && autoBright) {
        uint8_t brightScreen = map(getBright(), 0, 500, 10, 255);
        brightScreen = constrain(brightScreen, 10, 255);
        uint8_t brightLed = map(getBright(), 0, 1010, 10, 255);
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
    String time;
    String date;
    updateClock();

    getTime(&time, false);
    getDate(&date, true);
    
    showTime(time, date, true);
    setLedRGB(255, 255, 255);
    setLedMode(GLOW_MODE); 
}

void sensorView() {
    float pressure = getPressure() * 0.7506;

    showSensorData(getTemperature(), pressure);

    if (pressure <= 745) {
        setLedRGB(0, 0, 255);
        setLedMode(BLINK_MODE);
        setBlinkingSettings(1, 500, 500);
    }
    else if (pressure >= 746 && pressure <= 749) {
        setLedRGB(0, 0, 255);
        setLedMode(BREATHE_MODE);
    }
    else if (pressure >= 750 && pressure <= 770) {
        setLedRGB(0, 255, 0);
        setLedMode(GLOW_MODE);
    }
    else if (pressure >= 771 && pressure <= 775) {
        setLedRGB(175, 255, 0);
        setLedMode(GLOW_MODE);
    }
    else if (pressure > 775) {
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
        if ((timerTicking || timerPause) && selected < 2) selected++;
        else if (!timerTicking && !timerPause && selected < 4) selected++;
    }

    if (!timerPause && !timerTicking) {
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

                    timerTicking = true;
                    selected = 2;
                }
                break;
        }
    }
    else if (timerPause && !timerTicking) {
        switch (selected)
        {
            case 0: // Button Back
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    timer1.stop();
                    timer1.clear();
                    timerTicking = false;
                    timerPause = false;
                    selected = 4;
                }
                break;
            case 2:
                if (enc.click()) {
                    timer1.play();
                    timerPause = false;
                    timerTicking = true;
                }
                break;
        }
        hours = timer1.getHours();
        minutes = timer1.getMinutes();
        seconds = timer1.getSeconds();
    }
    else if (timerTicking && !timerPause) {
        switch (selected)
        {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    timer1.stop();
                    timer1.clear();
                    timerTicking = false;
                    selected = 4;
                }
                break;
            case 2:
                if (enc.click()) {
                    timer1.stop();
                    timerPause = true;
                    timerTicking = false;
                }
                break;
        }
        hours = timer1.getHours();
        minutes = timer1.getMinutes();
        seconds = timer1.getSeconds();
    }

    String time;
    time += (hours < 10) ? ('0' + String(hours)) : String(hours);
    time += ':';
    time += (minutes < 10) ? ('0' + String(minutes)) : String(minutes);
    time += ':';
    time += (seconds < 10) ? ('0' + String(seconds)) : String(seconds);

    showTimer(selected, time, timerTicking, timerPause, isShowTime);
}

void stopwatchView() {
    static uint8_t selected;

    if (!stopwatchPause && !stopwatchTicking) {
        if (enc.right() && selected < 1) selected++;
        if (enc.left() && selected > 0) selected--;

        switch (selected) {
            case 0:
                if (enc.click()) isMenu = false;
                break;
            case 1:
                if (enc.click()) {
                    stopwatch.play();
                    stopwatchTicking = true;
                    selected = 2;
                }
                break;
        }
    }
    else if (stopwatchPause && !stopwatchTicking) {
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
                    stopwatchPause = false;
                }
                break;
            case 2:
                if (enc.click()) {
                    stopwatch.play();
                    stopwatchTicking = true;
                    stopwatchPause = false;
                }
                break;
        }
    }
    else if (stopwatchTicking && !stopwatchPause) {
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
                    stopwatchTicking = false;
                }
                break;
            case 2:
                if (enc.click()) {
                    stopwatch.stop();
                    stopwatchPause = true;
                    stopwatchTicking = false;
                }
                break;
        }
    }

    String time;
    time += (stopwatch.getHours() < 10) ? ('0' + String(stopwatch.getHours())) : String(stopwatch.getHours());
    time += ':';
    time += (stopwatch.getMinutes() < 10) ? ('0' + String(stopwatch.getMinutes())) : String(stopwatch.getMinutes());
    time += ':';
    time += (stopwatch.getSeconds() < 10) ? ('0' + String(stopwatch.getSeconds())) : String(stopwatch.getSeconds());

    showStopwatch(time, selected, stopwatchTicking, stopwatchPause);
}

bool notAdded = true;
void menuView() {
    
    if (isMenu) {
        switch (menuState)
        {
            case 0: // Timer
                timerView();
                break;
            // case 1: // Alarm
            //     alarmView();
            //     break;
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
    timerPause = false;
    timerTicking = false;

    autoBright = false;
    setScreenBright(255);
    showTimerAlarm();
    setLedBrightness(255);
    setLedRGB(255, 0, 0);
    setBlinkingSettings(4, 75, 1000);
    setLedMode(BLINK_MODE);
    alarm();

    if (enc.click()) {
        autoBright = true;
        timerActivated = false;
    }
}

void defaultView() {
    showDefault();
}