#pragma once
#include <Arduino.h>
#include <stdio.h>

class Clock {
    private:
        uint8_t data[7];

    public:

        inline uint8_t getSeconds() const { return data[0]; }
        inline void setSeconds(uint8_t value) { data[0] = value; }

        inline uint8_t getMinutes() const { return data[1]; }
        inline void setMinutes(uint8_t value) { data[1] = value; }

        inline uint8_t getHours() const { return data[2]; }
        inline void setHours(uint8_t value) { data[2] = value; }

        inline uint8_t getDay() const { return data[3]; }
        inline void setDay(uint8_t value) { data[3] = value; }

        inline uint8_t getMonth() const { return data[4]; }
        inline void setMonth(uint8_t value) { data[4] = value; }

        inline uint8_t getYear() const { return data[5]; }
        inline void setYear(uint8_t value) { data[5] = value; }

        inline uint8_t getWeekday() const { return data[6]; }
        inline void setWeekday(uint8_t value) { data[6] = value; }

        void returnTimeAsString(char* buffer, uint8_t bufferSize, bool showSeconds = false) {
            if (showSeconds) {
                snprintf(buffer, bufferSize, "%02d:%02d:%02d", getHours(), getMinutes(), getSeconds());
            } else {
                snprintf(buffer, bufferSize, "%02d:%02d", getHours(), getMinutes());
            }
        }

        void returnDateAsString(char* buffer, uint8_t bufferSize) {
            snprintf(buffer, bufferSize, "%02d.%02d.20%02d", getDay(), getMonth(), getYear());
        }

        void returnWeekdayAsString(char* buffer, uint8_t bufferSize) {
            const char* days[] = {"", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

            uint8_t weekday = getWeekday();
            if (weekday >= 1 && weekday <= 7) {
                strncpy(buffer, days[weekday], bufferSize - 1);
                buffer[bufferSize - 1] = '\0';
            } else {
                buffer[0] = '\0';
            }
        }

        void syncFromRTC(DS3231& rtc) {
            setSeconds(rtc.getSeconds());
            setMinutes(rtc.getMinutes());
            setHours(rtc.getHours());
            setDay(rtc.getDay());
            setMonth(rtc.getMonth());
            setYear(rtc.getYear());
            setWeekday(rtc.getWeekday());
        }

        void syncToRTC(DS3231& rtc) {
            rtc.setSeconds(getSeconds());
            rtc.setMinutes(getMinutes());
            rtc.setHours(getHours());
            rtc.setDay(getDay());
            rtc.setMonth(getMonth());
            rtc.setYear(getYear());
            rtc.setWeekday(getWeekday());
        }
};

static_assert(sizeof(Clock) == 7, "Clock size is not 7 bytes!");