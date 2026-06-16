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

// BMP280
#define SENSOR_ADDRESS 0x76
BMP280 sens(SENSOR_ADDRESS);

// Photoresistor
#define PHOTO_PIN A0

// Display functions
void showTime(String& time, String& date, bool isAmFormat);
void screenSetup();

// RGB functions
void ledSetup();
void setLedRGB(uint8_t red, uint8_t green, uint8_t blue);
void setLedHue(uint8_t hue);

// RTC functions
void rtcSetup();
void getTime(String *buffer, bool sec);
void getDate(String *buffer, bool withWeekday);
void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void setDate(uint8_t day, uint8_t month, uint8_t year);
void updateClock();

void setup() {
    screenSetup();
    ledSetup();
    rtcSetup();

    Serial.begin(9600);

    sens.init();

    setLedRGB(255, 255, 255);
}

void loop() {
    enc.tick();

    static Timer clockUpdateTimer(500);

    if (clockUpdateTimer.ready()) {
        updateClock();
    
        String time;
        String date;
    
        getTime(&time, false);
        getDate(&date, true);
    
        showTime(time, date, true);
    }
}
