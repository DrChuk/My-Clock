#include <Arduino.h>
#include <Wire.h>
#include "RTC.h"

bool DS3231::init() {
    Wire.begin();

    return true;
}

uint8_t DS3231::getSeconds() {
    uint8_t value = read8(RTC_SECONDS_REGISTER);
    
    return ((value & 0x70) >> 4) * 10 + (value & 0x0F);
}

uint8_t DS3231::getMinutes() {
    uint8_t value = read8(RTC_MINUTES_REGISTER);
    
    return ((value & 0x70) >> 4) * 10 + (value & 0x0F);
}

uint8_t DS3231::getHours() {
    uint8_t value = read8(RTC_HOURS_REGISTER);

    uint8_t hours = 0;

    // Checking a format that we received from RTC
    if (((value & 0x40) >> 6)) {
        hours = ((value & 0x10) >> 4) * 10 + (value & 0x0F);
        if ((value & 0x20) >> 5) {
            hours += 12;
        }
    }
    else {
        hours = ((value & 0x30) >> 4) * 10 + (value & 0x0F);
    }

    return hours;
}

uint8_t DS3231::getWeekday() {
    uint8_t value = read8(RTC_WEEKDAY_REGISTER);
    return value;
}

uint8_t DS3231::getDay() {
    uint8_t value = read8(RTC_DAY_REGISTER);
    return ((value & 0x30) >> 4) * 10 + (value & 0x0F);
}

uint8_t DS3231::getMonth() {
    uint8_t value = read8(RTC_MONTH_REGISTER);
    return ((value & 0x01) >> 4) * 10 + (value & 0x0F);
}

uint8_t DS3231::getYear() {
    uint8_t value = read8(RTC_YEAR_REGISTER);
    return ((value & 0xF0) >> 4) * 10 + (value & 0x0F);
}

bool DS3231::setSeconds(uint8_t value) {
    uint8_t bits = ((value / 10) << 4) | (value % 10);
    return write8(RTC_SECONDS_REGISTER, bits);
}

bool DS3231::setMinutes(uint8_t value) {
    uint8_t bits = ((value / 10) << 4) | (value % 10);
    return write8(RTC_MINUTES_REGISTER, bits);
}

bool DS3231::setHours(uint8_t value) {
    uint8_t bits = 0b00 | ((value / 10) << 4) | (value % 10);
    return write8(RTC_HOURS_REGISTER, bits);
}

bool DS3231::setDay(uint8_t value) {
    uint8_t bits = 0b00 | ((value / 10) << 4) | (value % 10);
    return write8(RTC_DAY_REGISTER, bits);
}

bool DS3231::setMonth(uint8_t value) {
    uint8_t bits = 0b000 | ((value / 10) << 4) | (value % 10);
    return write8(RTC_MONTH_REGISTER, bits);
}

bool DS3231::setYear(uint8_t value) {
    uint8_t bits = ((value / 10) << 4) | (value % 10);
    return write8(RTC_YEAR_REGISTER, bits);
}

bool DS3231::setWeekday(uint8_t value) {
    uint8_t bits = value;
    return write8(RTC_WEEKDAY_REGISTER, bits);
}

uint8_t DS3231::read8(uint8_t reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);

    if (Wire.endTransmission(true) != 0) return RTC_ERROR;

    Wire.requestFrom(address, 1);
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