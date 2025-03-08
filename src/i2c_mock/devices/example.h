
#ifndef I2C_MOCK_ADXL_H
#define I2C_MOCK_ADXL_H

#include "i2c_device.h"

// This is an example of a really simple memory device that has one register
// To hold some memory. It is used to check that the i2c_mock works properly.

// Write procedure
// reg_addr = 0 => set bits enabled
// reg_addr = 1 => clear bits enabled
// reg_addr = 2 => set reg with data

// Read procedure
// reg_addr = 0 => read value
// reg_addr = 1 => read !value

// Read device will just do the read / write

#define REG_ENABLE 0
#define REG_CLEAR 1
#define REG_SET 2

#define REG_READ 0
#define REG_READ_NOT 1

struct ExampleI2CDevice : I2CDevice {
private:
    uint8_t reg = 0;
public:
    uint8_t i2c_read  (uint8_t reg_addr, uint8_t* data, uint32_t len) override {
        if (len == 0) return 0;

        if (reg_addr == REG_READ) data[0] = reg;
        if (reg_addr == REG_READ_NOT) data[0] = 0xFF ^ reg;

        return 0;
    }
    uint8_t i2c_write (uint8_t reg_addr, const uint8_t* data, uint32_t len) override {
        if (len == 0) return 0;

        if (reg_addr == REG_ENABLE) reg |= data[0];
        if (reg_addr == REG_CLEAR) reg &= 0xFF ^ data[0];
        if (reg_addr == REG_SET) reg  = data[0];

        return 0;
    }

    uint8_t i2c_read_device  (uint8_t* data, uint32_t len) override {
        if (len == 0) return 0;

        data[0] = reg;
        return 0;
    }
    uint8_t i2c_write_device (const uint8_t* data, uint32_t len) override {
        if (len == 0) return 0;

        reg = data[0];
        return 0;
    }
};

#endif
