#pragma once
#include <Arduino.h>
#include <Timer.h>
#include <MClasses.h>

class ClockAlarm {
    private:
        uint8_t hour : 5;
        uint8_t minute : 6;
        bool isTurned = false;
        Timer checkTimer = Timer(3000);
        Timer cdTimer = Timer(60000);

        void (*_callback)() = nullptr;
    public:
        void setTime(uint8_t hours, uint8_t minutes) { hours = hours; minute = minutes; }

        uint8_t getHour() { return hour; }
        uint8_t getMinute() { return minute; }
        bool getTurned() { return isTurned; }
        void setTurned(bool value) { isTurned = value; }

        void tick(Clock& clock) {
            if (checkTimer.ready() && isTurned) {
                if (clock.getHours() == hour && clock.getMinutes() == minute) {
                    if (_callback) _callback();
                }
            }
        }

        void attach(void (*callback)()) { _callback = callback; }
};