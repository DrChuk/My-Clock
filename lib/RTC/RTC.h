#pragma once
#include <Arduino.h>

#define RTC_ERROR 0xFF

#define RTC_SECONDS_REGISTER 0x00
#define RTC_MINUTES_REGISTER 0x01
#define RTC_HOURS_REGISTER 0x02
#define RTC_WEEKDAY_REGISTER 0x03
#define RTC_DAY_REGISTER 0x04
#define RTC_MONTH_REGISTER 0x05
#define RTC_YEAR_REGISTER 0x06

class DS3231 {
    public:
        DS3231(uint8_t address) : address(address) {}
        bool init();

        uint8_t getSeconds();
        uint8_t getMinutes();
        uint8_t getHours();
        uint8_t getWeekday();
        uint8_t getDay();
        uint8_t getMonth();
        uint8_t getYear();

        bool setSeconds(uint8_t value);
        bool setMinutes(uint8_t value);
        bool setHours(uint8_t value);
        bool setDay(uint8_t value);
        bool setMonth(uint8_t value);
        bool setYear(uint8_t value);
        bool setWeekday(uint8_t value);
    private:
        const uint8_t address;

        uint8_t read8(uint8_t reg);
        bool write8(uint8_t reg, uint8_t data);
};