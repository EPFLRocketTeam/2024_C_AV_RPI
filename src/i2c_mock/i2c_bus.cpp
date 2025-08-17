
#include "i2c_bus.h"

void I2CBus::clear () {
    devices.clear();
    devices.resize(1 << sizeof(uint8_t), nullptr);
    
    device_open.clear();
    device_open.resize(1 << sizeof(uint8_t), false);
}
void I2CBus::addDevice (uint8_t addr, I2CDevice* device) {
    devices[addr] = device;
}

uint8_t I2CBus::i2c_open (uint8_t addr) {
    if (devices[addr] == nullptr) return I2C_BUS_E_OPEN;
    if (device_open[addr]) return I2C_BUS_E_ADDRESS_ALREADY_IN_USE;
    device_open[addr] = true;
    return 0;
}
uint8_t I2CBus::i2c_close (uint8_t addr) {
    if (devices[addr] == nullptr) return I2C_BUS_E_CLOSE;
    device_open[addr] = false;
    return 0;
}

uint8_t I2CBus::i2c_read  (uint8_t addr, uint8_t reg_addr, uint8_t* data, uint32_t len) {
    if (!device_open[addr]) return I2C_BUS_E_NOT_OPEN;

    I2CDevice* device = devices[addr];
    if (device->i2c_read(reg_addr, data, len) != 0) return I2C_BUS_E_READ;
    
    return 0;
}
uint8_t I2CBus::i2c_write (uint8_t addr, uint8_t reg_addr, const uint8_t* data, uint32_t len) {
    if (!device_open[addr]) return I2C_BUS_E_NOT_OPEN;

    I2CDevice* device = devices[addr];
    if (device->i2c_write(reg_addr, data, len) != 0) return I2C_BUS_E_WRITE;
    
    return 0;
}

uint8_t I2CBus::i2c_read_device  (uint8_t addr, uint8_t* data, uint32_t len) {
    if (!device_open[addr]) return I2C_BUS_E_NOT_OPEN;

    I2CDevice* device = devices[addr];
    if (device->i2c_read_device(data, len) != 0) return I2C_BUS_E_READ;
    
    return 0;
}
uint8_t I2CBus::i2c_write_device (uint8_t addr, const uint8_t* data, uint32_t len) {
    if (!device_open[addr]) return I2C_BUS_E_NOT_OPEN;

    I2CDevice* device = devices[addr];
    if (device->i2c_write_device(data, len) != 0) return I2C_BUS_E_WRITE;
    
    return 0;
}
