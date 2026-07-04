#pragma once
#include <Arduino.h>
#include <Timer.h>
#include <MClasses.h>

class ClockAlarm {
    private:
        uint8_t hour : 5;
        uint8_t minute : 6;
        bool isTurned = false;
        bool canAlarm = true;
        Timer checkTimer = Timer(3000);
        Timer cdTimer = Timer(60000);

        void (*_callback)() = nullptr;
    public:
        void setTime(uint8_t hours, uint8_t minutes) { hour = hours; minute = minutes; }

        uint8_t getHour() { return hour; }
        uint8_t getMinute() { return minute; }
        bool getTurned() { return isTurned; }
        void setTurned(bool value) { isTurned = value; }

        void tick(uint8_t hours, uint8_t minutes) {
            if (cdTimer.ready() && !canAlarm) canAlarm = true;

            if (checkTimer.ready() && isTurned && canAlarm) {
                if (hour == hours && minute == minutes) {
                    if (_callback) {
                        canAlarm = false;
                        cdTimer.restart();
                        _callback();
                    }
                }
            }
        }

        void attach(void (*callback)()) { _callback = callback; }
};