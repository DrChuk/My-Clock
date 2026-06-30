#pragma once
#include <Arduino.h>

#define ERROR 0xFF

#define ID_VALUE 0x58
#define RESET_VALUE 0xB6

#define ID_REG 0xD0
#define RESET_REG 0xE0
#define CTRL_MEAS_REG 0xF4
#define CONFIG_REG 0xF5

#define PRESSURE_MSB 0xF7
#define PRESSURE_LSB 0xF8
#define PRESSURE_XLSB 0xF9
#define TEMPERATURE_MSB 0xFA
#define TEMPERATURE_LSB 0xFB
#define TEMPERATURE_XLSB 0xFC

enum PRESS_OVERSAMPLING_VALUES : uint8_t {
    x1 = 0b001,
    x2 = 0b010,
    x4 = 0b011,
    x8 = 0b100,
    x16 = 0b101
};

enum BMP_MODES : uint8_t {
    SLEEP = 0b00,
    FORCE = 0b01,
    NORMAL = 0b11
};

typedef struct {
    uint16_t dig_t1 = 0;
    int16_t dig_t2 = 0;
    int16_t dig_t3 = 0;
    uint16_t dig_p1 = 0;
    int16_t dig_p2 = 0;
    int16_t dig_p3 = 0;
    int16_t dig_p4 = 0;
    int16_t dig_p5 = 0;
    int16_t dig_p6 = 0;
    int16_t dig_p7 = 0;
    int16_t dig_p8 = 0;
    int16_t dig_p9 = 0;
} BMP_COEFFS;

class BMP280 {
    public:
        BMP280(uint8_t address) : address(address) {}
        bool init();
        
        bool measure();
        int getTemperature();
        int getPressure();

        void setSeaLevel(uint16_t value) {sea_level = value;}
    private:
        const uint8_t address;
        BMP_COEFFS _coeffs;

        uint16_t sea_level = 0;

        int temperature = 0;
        int pressure = 0;
        
        uint8_t read8(uint8_t reg);
        bool write8(uint8_t reg, uint8_t data);

        
        bool checkConnection();
        void reset();
        
        void readCoefficients();
        void burstRead(uint8_t startReg, uint8_t *buffer, uint8_t len);

        void setControlMeasurement(PRESS_OVERSAMPLING_VALUES temp_oversampling, PRESS_OVERSAMPLING_VALUES press_oversampling, BMP_MODES mode);
        void setConfig();
        int32_t compensateTemperature(int32_t rawTemp);
        uint32_t compensatePressure(int32_t rawPressure);
};