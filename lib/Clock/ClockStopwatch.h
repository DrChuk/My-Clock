#pragma once
#include <Arduino.h>
#include <Timer.h>

class Stopwatch {
    public:
        void tick() {
            if (updateTime.ready() && !isStoped) {
                if (seconds == 59) {
                    seconds = 0;
                    if (minutes == 59) {
                        minutes = 0;
                        if (hours == 99) hours = 0;
                        else hours++;
                    } else minutes++;
                } else seconds++;
            }
        }

        void play() { isStoped = false; }
        void stop() { isStoped = true; }
        void clear() { hours = 0; minutes = 0; seconds = 0; }

        uint8_t getHours() { return hours; }
        uint8_t getMinutes() { return minutes; }
        uint8_t getSeconds() { return seconds; }

    private:
        uint8_t hours : 7;
        uint8_t minutes : 6;
        uint8_t seconds : 6;

        bool isStoped = true;
        Timer updateTime = Timer(1000);
};