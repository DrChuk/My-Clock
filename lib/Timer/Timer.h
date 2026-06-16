#pragma once
#include <Arduino.h>

class Timer {
    public:
        Timer(uint32_t period) : period(period) {}
        bool ready() {
            if (millis() - timer >= period) {
                timer = millis();
                return true;
            }
            return false;
        }
    private:
        uint32_t timer, period;
};