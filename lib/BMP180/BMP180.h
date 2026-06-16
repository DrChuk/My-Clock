#pragma once
#include <Arduino.h>

enum : uint8_t {
    AC1_REG = 0xAA,
    AC2_REG = 0xAC,
    AC3_REG = 0xAE,
    AC4_REG = 0xB0,
    AC5_REG = 0xB2,
    AC6_REG = 0xB4,
    B1_REG = 0xB6,
    B2_REG = 0xB8,
    MB_REG = 0xBA,
    MC_REG = 0xBC,
    MD_REG = 0xBE
} BMP_REGS;

typedef struct {
    int16_t ac1 = 0;
    int16_t ac2 = 0;
    int16_t ac3 = 0;
    uint16_t ac4 = 0;
    uint16_t ac5 = 0;
    uint16_t ac6 = 0;

    int16_t b1 = 0;
    int16_t b2 = 0;

    int16_t mb = 0;
    int16_t mc = 0;
    int16_t md = 0;
} 
BMP_COEFS;

class BMP180 {
    public:
        BMP180(uint8_t address) : address(address) {}
        bool init();
        float getTempearature();
        void setTemperatureError(uint8_t value);
        void setTemperatureError(uint16_t value);
        void setTemperatureError(float value);
    private:
        uint8_t address;
        float temp_error = 0;

        // int16_t coefficients[8];
        // uint16_t ucoefficients[3];
        // int16_t coefficients[11];

        BMP_COEFS _coefs;
        const uint8_t MEASUREMENT_CONTROL = 0xF4;
        const uint8_t OUT_MSB = 0xF6;
        const uint8_t OUT_LSB = 0xF7;
        const uint8_t TEMP_ADDR = 0x2E;

        const uint8_t ERROR = 0xFF;

        uint16_t read16(uint8_t reg);
        uint8_t read8(uint8_t reg);
        bool write8(uint8_t reg, uint8_t data);

        bool readCoefficients();
        uint16_t readRawTemperature();
        int32_t computeB5(int32_t UT);
};