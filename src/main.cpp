#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <EncButton.h>
#include <SoftwareSerial.h>
#include <BMP280.h>
#include <MClasses.h>
#include <Timer.h>

// 0x27 - LCD, 0x57 - ?, 0x68 - DS3231 (RTC), 0x77 - BMP280 (Temperature Sensor)

// Encoder
#define S1 2
#define S2 4
#define KEY 7
EncButton enc(S1, S2, KEY);

// Buzzer
#define BUZZER 5

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
void showTime(String& time, String& date, bool isAmFormat);
void showSensorData(float temp, float pressure);
void test();

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

void setup() {
    enc.setEncType(EB_STEP4_LOW);
    screenSetup();
    ledSetup();
    rtcSetup();
    sensorSetup();
    setSeaLevel(185);

    Serial.begin(9600);

    setLedMode(2);
    // test();
}

void navigateView();
uint8_t viewState = 0;

void loop() {
    enc.tick();
    screenLoop();
    ledLoop();
    navigateView();

    static Timer brightTimer(150);

    if (brightTimer.ready()) {
        uint8_t brightScreen = map(getBright(), 0, 500, 10, 255);
        uint8_t brightLed = map(getBright(), 0, 1010, 10, 255);
        setScreenBright(brightScreen);
        setLedBrightness(brightLed);
    }

}

void navigateView() {
    static Timer clockUpdateTimer(500);

    if (enc.left() && viewState > 0) {
        String debug;
        debug += "Direction: Left, View State: ";
        debug += viewState;
        Serial.println(debug);
        viewState--;
    }
    if (enc.right() && viewState < 1) {
        String debug;
        debug += "Direction: Right, View State: ";
        debug += viewState;
        Serial.println(debug);
        viewState++;
    }

    if (viewState == 0) {
        String time;
        String date;
        updateClock();

        getTime(&time, false);
        getDate(&date, true);
        
        showTime(time, date, true);
        setLedRGB(255, 255, 255);
        setLedMode(BREATHE_MODE); 
    }
    else if (viewState == 1) {
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
}