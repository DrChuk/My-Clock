#pragma once
#include <Arduino.h>

class Clock {
    public:
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
        uint8_t day;
        uint8_t month;
        uint8_t year;
        uint8_t weekday;

        void returnTimeAsString(String* buffer, bool sec) {
            String value;

            value = (hours < 10) ? ("0" + (String)hours) : (String)hours;
            value += ":" + ((minutes < 10) ? ("0" + (String)minutes) : (String)minutes);
            if (sec)
                value += ":" + ((seconds < 10) ? ("0" + (String)seconds) : (String)seconds);

            *buffer = value;
        }

        void returnDateAsString(String* buffer) {
            String value;

            value = (day < 10) ? ("0" + (String)day) : (String)day;
            value += "." + ((month < 10) ? ("0" + (String)month) : (String)month);
            value += ".20" + ((year < 10) ? ("0" + (String)year) : (String)year);

            *buffer = value;
        }

        void returnWeekdayAsString(String* buffer) {
            String value;

            switch (weekday) {
                case 1:
                    value = "Sun";
                    break;
                case 2:
                    value = "Mon";
                    break;
                case 3:
                    value = "Tue";
                    break;
                case 4:
                    value = "Wed";
                    break;
                case 5:
                    value = "Thu";
                    break;
                case 6:
                    value = "Fri";
                    break;
                case 7:
                    value = "Sat";
                    break;
            }

            *buffer = value;
        }

        void returnClockAsString(String* buffer) {
            String value;

            value = (hours < 10) ? ("0" + (String)hours) : (String)hours;
            value += ":" + ((minutes < 10) ? ("0" + (String)minutes) : (String)minutes);
            value += ":" + ((seconds < 10) ? ("0" + (String)seconds) : (String)seconds);
            value += ", ";
            value += (day < 10) ? ("0" + (String)day) : (String)day;
            value += "." + ((month < 10) ? ("0" + (String)month) : (String)month);
            value += "." + ((year < 10) ? ("0" + (String)year) : (String)year);
            value += ", ";
            value += (weekday < 10) ? ("0" + (String)weekday) : (String)weekday;

            *buffer = value;
        }
};