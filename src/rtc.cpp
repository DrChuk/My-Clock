#include <RTC.h>
#include <MClasses.h>
#include <Timer.h>

Clock clock;
DS3231 rtc(0x68);

void rtcSetup() {
    rtc.init();
}

void getTime(String *buffer, bool sec) {
    clock.returnTimeAsString(buffer, sec);
}

void getDate(String *buffer, bool withWeekday) {
    if (!withWeekday) clock.returnDateAsString(buffer);
    else {
        String date;
        String weekday;
        clock.returnDateAsString(&date);
        clock.returnWeekdayAsString(&weekday);

        *buffer = date + ", " + weekday;
    }
}

void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    rtc.setHours(hours);
    rtc.setMinutes(minutes);
    rtc.setSeconds(seconds);
}

void setDate(uint8_t day, uint8_t month, uint8_t year) {
    uint8_t monthNum[12] = {1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
    if (year != 0 && year % 4 == 0) {
        monthNum[0]--;
        monthNum[1]--;
    }

    uint16_t yearNum = (6 + year + year / 4) % 7;
    uint8_t weekday = (day + monthNum[month - 1] + yearNum) % 7;

    rtc.setDay(day);
    rtc.setMonth(month);
    rtc.setYear(year);
    rtc.setWeekday(weekday);
}

void updateClock() {
    clock.hours = rtc.getHours();
    clock.minutes = rtc.getMinutes();
    clock.seconds = rtc.getSeconds();
    clock.day = rtc.getDay();
    clock.month = rtc.getMonth();
    clock.year = rtc.getYear();
    clock.weekday = rtc.getWeekday();
}