
#include "i2c_bus.h"
#include "i2c_wrappers.h"
#include "i2c_interface.h"
#include "devices/example.h"
#include <stdio.h>

#define EXAMPLE_ADDRESS 0x42

int main (void) {
    ExampleI2CDevice device;
    I2CBus & bus = I2CBus::getInstance();
    bus.addDevice(EXAMPLE_ADDRESS, &device);

    I2CInterface & intf = I2CInterface::getInstance();

    if (i2c_open( EXAMPLE_ADDRESS ) != 0) return 1;
    
    void* intf_ptr;
    if (get_intf_ptr( EXAMPLE_ADDRESS, &intf_ptr) != 0) return 1;

    uint8_t buffer[1] = { 42 };
    if (i2c_read( REG_READ, buffer, 1, intf_ptr ) != 0) return 1;
    if (buffer[0] != 0) return 1;
    if (i2c_read( REG_READ_NOT, buffer, 1, intf_ptr ) != 0) return 1;
    if (buffer[0] != 255) return 1;

    buffer[0] = 0x43;
    if (i2c_write(REG_ENABLE, buffer, 1, intf_ptr) != 0) return 1;

    buffer[0] = 0;
    if (i2c_read( REG_READ, buffer, 1, intf_ptr ) != 0) return 1;
    if (buffer[0] != 0x43) return 1;

    buffer[0] = 0x42;
    if (i2c_write(REG_CLEAR, buffer, 1, intf_ptr) != 0) return 1;

    buffer[0] = 0;
    intf.readDevice( EXAMPLE_ADDRESS, buffer, 1 );
    if (buffer[0] != 0x01) return 1;

    buffer[0] = 0x42;
    if (i2c_write(REG_SET, buffer, 1, intf_ptr) != 0) return 1;

    buffer[0] = 0;
    if (i2c_read( REG_READ, buffer, 1, intf_ptr ) != 0) return 1;
    if (buffer[0] != 0x42) return 1;

    buffer[0] = 0x43;
    intf.writeDevice( EXAMPLE_ADDRESS, buffer, 1 );

    buffer[0] = 0;
    if (i2c_read( REG_READ_NOT, buffer, 1, intf_ptr ) != 0) return 1;
    if (buffer[0] != 0xBC) return 1;

    if (i2c_close(EXAMPLE_ADDRESS) != 0) return 1;
}
