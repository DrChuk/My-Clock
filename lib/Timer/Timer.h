#pragma once
#include <Arduino.h>

class Timer {
    public:
        Timer(uint32_t period) : period(period) {}
        bool ready() {
            if (millis() - timer >= period && turnedOn) {
                timer = millis();
                return true;
            }
            return false;
        }
        void restart() {
            timer = millis();
        }

        void enable() { turnedOn = true;}
        void disable() { turnedOn = false; }

        void setPeriod(uint32_t value) {
            period = value;
        }
    private:
        uint32_t timer, period;
        bool turnedOn = true;
};