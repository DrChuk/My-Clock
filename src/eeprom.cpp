#include <Arduino.h>
#include <EEPROM.h>
#include <ClockAlarm.h>
#include <Settings.h>

#define INIT_ADDRESS 0
#define INIT_KEY 0xFF

void firstOn(Settings& settings) {
    if (EEPROM.read(INIT_ADDRESS) != INIT_KEY) {
        EEPROM.put(INIT_ADDRESS, INIT_KEY);
        Settings nullSettings;

        EEPROM.put(1, nullSettings);
    }
    EEPROM.get(1, settings);
}

void settingsToDefault() {
    EEPROM.write(0, 0);
}

void saveSettings(Settings& settings) {
    EEPROM.put(1, settings);
}