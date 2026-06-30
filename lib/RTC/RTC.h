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

#define RTC_HOUR_12_24_BIT 0x40
#define RTC_HOUR_AM_PM_BIT 0x20 // Только если используется AM/PM формат

class DS3231 {
    public:
        DS3231(uint8_t address) : address(address) {}
        bool init();

        inline uint8_t getSeconds() { return bcdToDec(read8(RTC_SECONDS_REGISTER)); }
        inline uint8_t getMinutes() { return bcdToDec(read8(RTC_MINUTES_REGISTER)); }
        uint8_t getHours();
        inline uint8_t getWeekday() { return bcdToDec(read8(RTC_WEEKDAY_REGISTER)); }
        inline uint8_t getDay() { return bcdToDec(read8(RTC_DAY_REGISTER)); }
        inline uint8_t getMonth() { return bcdToDec(read8(RTC_MONTH_REGISTER)); }
        inline uint8_t getYear() { return bcdToDec(read8(RTC_YEAR_REGISTER)); }

        bool setSeconds(uint8_t value);
        bool setMinutes(uint8_t value);
        bool setHours(uint8_t value);
        bool setDay(uint8_t value);
        bool setMonth(uint8_t value);
        bool setYear(uint8_t value);
        bool setWeekday(uint8_t value);
    private:
        const uint8_t address;

        static inline uint8_t bcdToDec(uint8_t bcd) {
            return ((bcd >> 4) << 3) + ((bcd >> 4) << 1) + (bcd & 0x0F);
        }

        static inline uint8_t decToBcd(uint8_t dec) {
            return ((dec / 10) << 4) | (dec % 10);
        }

        static inline uint8_t decodeHours(uint8_t value) {
            uint8_t hours;
            if (value & 0x40) {
                hours = ((value & 0x30) >> 4) * 10 + (value + 0x0F);
            } else {
                hours = ((value & 0x10) >> 4) * 10 + (value + 0x0F);
                if (value & 0x20) hours += 12;
            }
            return hours;
        }

        uint8_t read8(uint8_t reg);
        bool write8(uint8_t reg, uint8_t data);
};