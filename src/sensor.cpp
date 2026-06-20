#include <Arduino.h>
#include <BMP280.h>

BMP280 sens(0x76);

void sensorSetup() {
    sens.init();
}

void setSeaLevel(float seaLevel) {
    sens.setSeaLevel(seaLevel);
}

float getTemperature() {
    sens.measure();
    return sens.getTemperature();
}

float getPressure() {
    sens.measure();
    return sens.getPressure();
}

