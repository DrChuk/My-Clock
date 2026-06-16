#include <Arduino.h>
#include "BMP180.h"
#include <Wire.h>

bool BMP180::init() {
    Wire.begin();

    return readCoefficients();
}

float BMP180::getTempearature() {
    uint16_t UT = readRawTemperature();

    if (UT == ERROR) return ERROR;
    return ((float)(computeB5(UT) + 8) / 160) - temp_error;
}

void BMP180::setTemperatureError(uint8_t value) {
    temp_error = (float)value;
}

void BMP180::setTemperatureError(uint16_t value) {
    temp_error = (float)value;
}

void BMP180::setTemperatureError(float value) {
    temp_error = value;
}

uint16_t BMP180::readRawTemperature() {
    if (!write8(MEASUREMENT_CONTROL, TEMP_ADDR)) return ERROR;

    delay(5);

    return read16(OUT_MSB);
}

bool BMP180::readCoefficients() {
    uint32_t value = 0;

    for (uint8_t reg = AC1_REG; reg <= MD_REG; reg++) {
        value = read16(reg);

        if (value == ERROR) return false;

        switch (reg) {
            case AC1_REG:
                _coefs.ac1 = value;
                break;
            case AC2_REG:
                _coefs.ac2 = value;
                break;
            case AC3_REG:
                _coefs.ac3 = value;
                break;
            case AC4_REG:
                _coefs.ac4 = value;
                break;
            case AC5_REG:
                _coefs.ac5 = value;
                break;
            case AC6_REG:
                _coefs.ac6 = value;
                break;
            case B1_REG:
                _coefs.b1 = value;
                break;
            case B2_REG:
                _coefs.b2 = value;
                break;
            case MB_REG:
                _coefs.mb = value;
                break;
            case MC_REG:
                _coefs.mc = value;
                break;
            case MD_REG:
                _coefs.md = value;
                break;
        }
    }

    return true;
}

int32_t BMP180::computeB5(int32_t UT) {
    int32_t X1 = ((UT - (int32_t)_coefs.ac6) * _coefs.ac5 >> 15);
    int32_t X2 = (_coefs.mc << 11) / (((UT - _coefs.ac6) * _coefs.ac5 >> 15) + _coefs.md);

    return X1 + X2;
}

uint16_t BMP180::read16(uint8_t reg) {
    uint16_t value = 0;

    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission(true) != 0) return ERROR;

    Wire.requestFrom(address, 2);

    if (Wire.available() != 2) return ERROR;

    value = Wire.read() << 8;
    value |= Wire.read();

    return value;
}

uint8_t BMP180::read8(uint8_t reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission(true) != 0) return ERROR;

    Wire.requestFrom(address, 1);

    if (Wire.available() != 1) return ERROR;

    return Wire.read();
}

bool BMP180::write8(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data);

    if (Wire.endTransmission(true) == 0) return true;
    return false;
}