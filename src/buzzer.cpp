#include <Arduino.h>
#include <Timer.h>
#define BUZZER 5

bool quietMode = false;
Timer alarmTimer(125);
Timer cooldownTimer(1000);
bool cooldown = false;

void alarm() {
    static uint8_t counts;

    if (cooldownTimer.ready() && cooldown) cooldown = false;

    if (alarmTimer.ready() && !cooldown) {
        tone(BUZZER, 2000, 50);
        counts++;
        Serial.println(counts);
    }
    if (counts >= 4) {
        counts = 0;
        cooldown = true;
    }
}

void setQuietMode(bool value) {
    quietMode = value;
}

void switchQuietMode() {
    quietMode = !quietMode;
}

bool getQuietMode() {
    return quietMode;
}