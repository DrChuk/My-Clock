#include <Arduino.h>
#include <Wire.h>
#include "RTC.h"

bool DS3231::init() {
    Wire.begin();
    Wire.setClock(400000L);

    Wire.beginTransmission(address);
    if (Wire.endTransmission() != 0) return false;

    // uint8_t hourReg = read8(RTC_HOURS_REGISTER);
    // if (hourReg != RTC_ERROR) {
    //     hourReg |= RTC_HOUR_12_24_BIT;
    //     if (!write8(RTC_HOURS_REGISTER, hourReg)) {
    //         return false;  // Не удалось записать
    //     }
    // }

    return true;
}

uint8_t DS3231::getHours() {
    uint8_t value = bcdToDec(read8(RTC_HOURS_REGISTER));
    // Serial.println(value, BIN);
    if (value == RTC_ERROR) return RTC_ERROR;
    return value;
}

bool DS3231::setSeconds(uint8_t value) {
    if (value > 59) return false;
    return write8(RTC_SECONDS_REGISTER, decToBcd(value));
}

bool DS3231::setMinutes(uint8_t value) {
    if (value > 59) return false;
    return write8(RTC_MINUTES_REGISTER, decToBcd(value));
}

bool DS3231::setHours(uint8_t value) {
    if (value > 23) return false;
    uint8_t bcd = 0b00 | decToBcd(value);
    // Serial.println(bcd, BIN);
    return write8(RTC_HOURS_REGISTER, bcd);
}

bool DS3231::setDay(uint8_t value) {
    if (value < 1 || value > 31) return false;
    return write8(RTC_DAY_REGISTER, decToBcd(value));
}

bool DS3231::setMonth(uint8_t value) {
    if (value < 1 || value > 12) return false;
    return write8(RTC_MONTH_REGISTER, decToBcd(value));
}

bool DS3231::setYear(uint8_t value) {
    if (value < 1 || value > 99) return false;
    return write8(RTC_YEAR_REGISTER, decToBcd(value));
}

bool DS3231::setWeekday(uint8_t value) {
    if (value < 1 || value > 7) return false;
    return write8(RTC_WEEKDAY_REGISTER, value);
}

uint8_t DS3231::read8(uint8_t reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);

    if (Wire.endTransmission(true) != 0) return RTC_ERROR;

    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available() != 1) return RTC_ERROR;

    return Wire.read();
}

bool DS3231::write8(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data);

    if (Wire.endTransmission(true) == 0) return true;
    return false;
}