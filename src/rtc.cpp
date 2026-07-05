#include <RTC.h>
#include <MClasses.h>
#include <Timer.h>
#include <stdio.h>

Clock clock;
DS3231 rtc(0x68);

void rtcSetup() {
    rtc.init();
}

void getTime(char *buffer, uint8_t bufferSize, bool sec) {
    clock.returnTimeAsString(buffer, bufferSize, sec);
}

void getDate(char* buffer, uint8_t bufferSize, bool withWeekday) {
    if (!withWeekday) clock.returnDateAsString(buffer, bufferSize);
    else {
        char date[11] = "";
        char weekday[4] = "";

        clock.returnDateAsString(date, sizeof(date));
        clock.returnWeekdayAsString(weekday, sizeof(weekday));

        buffer[0] = '\0';
        strcat(buffer, date);
        strcat(buffer, ", ");
        strcat(buffer, weekday);
    }
}

void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    clock.setHours(hours);
    clock.setMinutes(minutes);
    clock.setSeconds(seconds);
}

uint8_t getWeekday(uint8_t day, uint8_t month, uint8_t year) {
    uint8_t monthNum[12] = {1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
    if (year != 0 && year % 4 == 0) {
        monthNum[0]--;
        monthNum[1]--;
    }

    uint8_t yearNum = (6 + year + year / 4) % 7;
    uint8_t weekday = (day + monthNum[month - 1] + yearNum) % 7;
    if (weekday == 0) weekday = 7;
    return weekday;
}

void setDate(uint8_t day, uint8_t month, uint8_t year) {
    clock.setDay(day);
    clock.setMonth(month);
    clock.setYear(year);
    clock.setWeekday(getWeekday(day, month, year));
}

void syncToClock() {
    clock.syncToRTC(rtc);
}

void syncFromClock() {
    clock.syncFromRTC(rtc);
}