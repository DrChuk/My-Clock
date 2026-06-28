#pragma once
#include <Arduino.h>
#include <Timer.h>

class ClockTimer {
    public:
        ClockTimer() {
            isPause = 1;
        }

        void setTime(uint8_t hour, uint8_t minute, uint8_t second) {
            hours = hour;
            minutes = minute;
            seconds = second;
        }

        void clear() {
            hours = 0;
            minutes = 0;
            seconds = 0;
        }

        uint8_t getSeconds() {
            return seconds;
        }

        uint8_t getMinutes() {
            return minutes;
        }

        uint8_t getHours() {
            return hours;
        }

        void tick() {
            if (updateTimer.ready() && !isPause) {
                if (seconds == 0) {
                    seconds = 59;
                    if (minutes == 0) {
                        minutes = 59;
                        if (hours == 0) {
                            if (_callback) _callback();
                        } else {
                            hours--;
                        }
                    } else {
                        minutes--;
                    }
                } else {
                    seconds--;
                }
            }
        }

        void play() {
            updateTimer.restart();
            isPause = false;
        }

        void stop() {
            isPause = true;
        }

        void attach(void (*callback)()) {
            _callback = callback;
        }
    private:
        uint8_t hours : 7;
        uint8_t minutes : 6;
        uint8_t seconds : 6;

        uint8_t isPause : 1;
        // isPause = 1;

        Timer updateTimer = Timer(1000);

        void (*_callback)() = nullptr;
};