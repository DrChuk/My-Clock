#include "BMP280.h"
#include <Wire.h>

bool BMP280::init() {
    Wire.begin();
    
    bool check = checkConnection();
    if (!check) return false;

    reset();
    delay(10);
    readCoefficients();
    setConfig();
    setControlMeasurement(x2, x16, NORMAL);

    return true;
}

uint32_t measure_timer;

bool out = 0;
bool BMP280::measure() {
    int32_t rawPressure = ((uint32_t)read8(PRESSURE_MSB) << 12) | ((uint32_t)read8(PRESSURE_LSB) << 4) | ((uint32_t)read8(PRESSURE_XLSB) >> 4);
    int32_t rawTemperature = (int32_t)(((uint32_t)read8(TEMPERATURE_MSB) << 12) | ((uint32_t)read8(TEMPERATURE_LSB) << 4) | ((uint32_t)(read8(TEMPERATURE_XLSB) & 0xF0) >> 4));

    temperature = compensateTemperature(rawTemperature) / 10;
    pressure = compensatePressure(rawPressure) / 100;
    
    measure_timer = millis();
    return true;
}

int BMP280::getTemperature() {
    return temperature;
}
int BMP280::getPressure() {
    return pressure * pow(1 - 0.0000225577 * sea_level, -5.255);;
}

int32_t t_fine = 0;
int32_t BMP280::compensateTemperature(int32_t rawTemp) {
    int32_t var1, var2, T;

    var1 = ((((rawTemp >> 3) - ((int32_t)_coeffs.dig_t1 << 1))) * ((int32_t)_coeffs.dig_t2)) >> 11;
    var2 = (((((rawTemp >> 4) - ((int32_t)_coeffs.dig_t1)) * ((rawTemp >> 4) - ((int32_t)_coeffs.dig_t1))) >> 12 ) * ((int32_t)_coeffs.dig_t3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;

    return T;
}

uint32_t BMP280::compensatePressure(int32_t rawPress) {
    int32_t var1, var2;
    uint32_t p;

    var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)_coeffs.dig_p6);
    var2 = var2 + ((var1 * ((int32_t)_coeffs.dig_p5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)_coeffs.dig_p4) << 16);
    var1 = (((_coeffs.dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)_coeffs.dig_p2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)_coeffs.dig_p1)) >> 15);

    if (var1 == 0) return 0;
    p = (((uint32_t)(((int32_t)1048576) - rawPress) - (var2 >> 12))) * 3125;
    if (p < 0x80000000) p = (p << 1) / ((uint32_t)var1);
    else p = (p / (uint32_t)var1) * 2;

    var1 = (((int32_t)_coeffs.dig_p9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(p >> 2)) * ((int32_t)_coeffs.dig_p8)) >> 13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + _coeffs.dig_p7) >> 4));

    return p;
}

bool BMP280::checkConnection() {
    uint8_t id = read8(ID_REG);

    return id == ID_VALUE;
}

void BMP280::reset() {
    write8(RESET_REG, RESET_VALUE);
    
    delay(3);
}

void BMP280::setControlMeasurement(PRESS_OVERSAMPLING_VALUES temp_oversampling, PRESS_OVERSAMPLING_VALUES press_oversampling, BMP_MODES mode) {
    uint8_t value = (temp_oversampling << 3) | (press_oversampling << 3) | mode;

    write8(CTRL_MEAS_REG, value);
}

void BMP280::setConfig() {
    uint8_t value = 0b0010000;
    
    write8(CONFIG_REG, value);
}

void BMP280::readCoefficients() {
    uint8_t data[24];

    burstRead(0x88, data, 24);

    _coeffs.dig_t1 = ((uint16_t)data[1] << 8) | data[0];
    _coeffs.dig_t2 = ((int16_t)data[3] << 8) | data[2];
    _coeffs.dig_t3 = ((int16_t)data[5] << 8) | data[4];
    
    _coeffs.dig_p1 = ((uint16_t)data[7] << 8) | data[6];
    _coeffs.dig_p2 = ((int16_t)data[9] << 8) | data[8];
    _coeffs.dig_p3 = ((int16_t)data[11] << 8) | data[10];
    _coeffs.dig_p4 = ((int16_t)data[13] << 8) | data[12];
    _coeffs.dig_p5 = ((int16_t)data[15] << 8) | data[14];
    _coeffs.dig_p6 = ((int16_t)data[17] << 8) | data[16];
    _coeffs.dig_p7 = ((int16_t)data[19] << 8) | data[18];
    _coeffs.dig_p8 = ((int16_t)data[21] << 8) | data[20];
    _coeffs.dig_p9 = ((int16_t)data[23] << 8) | data[22];
}

void BMP280::burstRead(uint8_t startReg, uint8_t *buffer, uint8_t len) {
    Wire.beginTransmission(address);
    Wire.write(startReg);
    Wire.endTransmission(false);

    Wire.requestFrom(address, len, true);
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
        buffer[i] = Wire.read();
    }
}

uint8_t BMP280::read8(uint8_t reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);

    if (Wire.endTransmission(true) != 0) return ERROR;

    Wire.requestFrom(address, 1);
    if (Wire.available() != 1) return ERROR;

    return Wire.read();
}

bool BMP280::write8(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data);

    if (Wire.endTransmission(true) == 0) return true;
    return false;
}