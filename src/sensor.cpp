#include <Arduino.h>
#include <BMP280.h>

BMP280 sens(0x76);

void sensorSetup() {
    sens.init();
}

void setSeaLevel(float seaLevel) {
    sens.setSeaLevel(seaLevel);
}

int getTemperature() {
    sens.measure();
    return sens.getTemperature();
}

int getPressure() {
    sens.measure();
    return sens.getPressure();
}

