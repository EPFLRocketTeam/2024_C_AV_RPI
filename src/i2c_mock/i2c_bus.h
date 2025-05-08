
#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <vector>
#include "i2c_device.h"

#include <cstdint>

#define I2C_BUS_OK 0
#define I2C_BUS_E_OPEN -1
#define I2C_BUS_E_CLOSE -2
#define I2C_BUS_E_READ -3
#define I2C_BUS_E_WRITE -4
#define I2C_BUS_E_ADDRESS_ALREADY_IN_USE -5
#define I2C_BUS_E_NOT_OPEN -6

struct I2CBus {
private:
    std::vector<I2CDevice*> devices;
    std::vector<bool> device_open;
    
    I2CDevice* getDevice (uint8_t addr);
public:
    void addDevice (uint8_t addr, I2CDevice* device);
    void clear();

    uint8_t i2c_open  (uint8_t addr);
    uint8_t i2c_close (uint8_t addr);

    uint8_t i2c_read  (uint8_t addr, uint8_t reg_addr, uint8_t* data, uint32_t len);
    uint8_t i2c_write (uint8_t addr, uint8_t reg_addr, const uint8_t* data, uint32_t len);

    uint8_t i2c_read_device  (uint8_t addr, uint8_t* data, uint32_t len);
    uint8_t i2c_write_device (uint8_t addr, const uint8_t* data, uint32_t len);

    static I2CBus& getInstance() {
        static I2CBus instance;
        instance.devices.resize(1 << sizeof(uint8_t), nullptr);
        instance.device_open.resize(1 << sizeof(uint8_t), false);

        return instance;
    }
};

#endif
