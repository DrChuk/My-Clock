#include <Arduino.h>
#include <ClockAlarm.h>

struct ClockAlarmSettings {
    uint8_t hour = 0;
    uint8_t minute = 0;
    bool isTurnedOn = false;
};

struct ClockAlarms {
    ClockAlarmSettings clock1;
    ClockAlarmSettings clock2;
    ClockAlarmSettings clock3;
    ClockAlarmSettings clock4;
};

struct Settings {
    ClockAlarms alarms;
    bool amPmFormat = false;
    bool autoBright = true;
    uint8_t brightness = 100;
    bool showSeconds = false;
};