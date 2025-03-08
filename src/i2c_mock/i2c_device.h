
#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include <vector>
#include <cstdint>

struct I2CDevice {
    virtual uint8_t i2c_read  (uint8_t reg_addr, uint8_t* data, uint32_t len) = 0;
    virtual uint8_t i2c_write (uint8_t reg_addr, const uint8_t* data, uint32_t len) = 0;

    virtual uint8_t i2c_read_device  (uint8_t* data, uint32_t len) = 0;
    virtual uint8_t i2c_write_device (const uint8_t* data, uint32_t len) = 0;
};

#endif
