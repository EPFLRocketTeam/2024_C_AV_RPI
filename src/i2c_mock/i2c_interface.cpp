
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstring>

#include "i2c_interface.h"
#include "i2c_wrappers.h"
#include "i2c_bus.h"

void I2CInterface::initialize() {
    std::memset(handle_list, 0, sizeof(handle_list));
    std::memset(i2c_addr_list, 0, sizeof(i2c_addr_list));
}

void I2CInterface::terminate() {
    std::memset(handle_list, 0, sizeof(handle_list));
    std::memset(i2c_addr_list, 0, sizeof(i2c_addr_list));
}

void I2CInterface::open(uint8_t addr) {
    if (I2CBus::getInstance().i2c_open(addr) != 0)
        throw I2CInterfaceException("Failed to open I2C device");
    handle_list[addr] = addr;
    i2c_addr_list[addr] = addr;
    usleep(10000); // Delay for stabilization
}

void I2CInterface::close(uint8_t addr) {
    if (I2CBus::getInstance().i2c_close(handle_list[addr]) != 0) {
        throw I2CInterfaceException("Failed to close I2C device");
    }
    handle_list[addr] = 0;
    i2c_addr_list[addr] = 0;
}

void I2CInterface::write(uint8_t addr, uint8_t reg_addr, const uint8_t* data, uint32_t len) {
    if (I2CBus::getInstance().i2c_write(addr, reg_addr, data, len) != 0)
        throw I2CInterfaceException("I2C write operation failed");
}

void I2CInterface::read(uint8_t addr, uint8_t reg_addr, uint8_t* data, uint32_t len) {
    if (I2CBus::getInstance().i2c_read(addr, reg_addr, data, len) != 0)
        throw I2CInterfaceException("I2C read operation failed");
}

void I2CInterface::readDevice(uint8_t addr, uint8_t* data, uint32_t len) {
    if (I2CBus::getInstance().i2c_read_device(addr, data, len) != 0)
        throw I2CInterfaceException("I2C read operation failed");
}

void I2CInterface::writeDevice(uint8_t addr, uint8_t* data, uint32_t len) {
    if (I2CBus::getInstance().i2c_write_device(addr, data, len) != 0)
        throw I2CInterfaceException("I2C write operation failed");
}

void* I2CInterface::get_intf_ptr(uint8_t addr) {
    if (i2c_addr_list[addr] == addr) {
        return &i2c_addr_list[addr];
    } else {
        throw I2CInterfaceException("I2C device at address not initialized");
    }
}

extern "C" {

int8_t i2c_open(uint8_t addr) {
    try {
        I2CInterface::getInstance().open(addr);
        return 0; // Success
    } catch (const I2CInterfaceException& e) {
        return COMM_FAIL;
    }
}

int8_t i2c_close(uint8_t addr) {
    try {
        I2CInterface::getInstance().close(addr);
        return 0; // Success
    } catch (const I2CInterfaceException& e) {
        return COMM_FAIL;
    }
}

void i2c_delay_us(uint32_t period, void* intf_ptr) {
    // Simple wrapper, exception handling is likely unnecessary here
    usleep(period);
}

int8_t i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    try {
        uint8_t addr = *(reinterpret_cast<uint8_t*>(intf_ptr)); // Assuming intf_ptr points to the address
        I2CInterface::getInstance().read(addr, reg_addr, reg_data, len);
        return 0; // Success
    } catch (const I2CInterfaceException& e) {
        return COMM_FAIL;
    }
}

int8_t i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    try {
        uint8_t addr = *(reinterpret_cast<uint8_t*>(intf_ptr));
        I2CInterface::getInstance().write(addr, reg_addr, reg_data, len);
        return 0; // Success
    } catch (const I2CInterfaceException& e) {
        return COMM_FAIL;
    }
}

int8_t get_intf_ptr(uint8_t addr, void **intf_ptr) {
    try {
        *intf_ptr = I2CInterface::getInstance().get_intf_ptr(addr);
        return 0;
    } catch (const I2CInterfaceException& e) {
        return COMM_FAIL;
    }
}

} //END OF C FUNCTIONS