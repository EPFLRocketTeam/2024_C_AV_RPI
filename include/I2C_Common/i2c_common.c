/*! @file i2c_common.c
 * @brief I2C function wrappers for use in other files.*/


#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>

#include "i2c_common.h"

/*!
 *  @brief handle list where each handle used by pigpio is stored.
 */
static int handle_list[0x7F];
/*!
 *  @brief address list where each address is stored.
 */
int i2c_addr_list[0x7F];

int8_t get_intf_ptr(uint8_t addr, void **intf_ptr) {
    if (i2c_addr_list[addr] == addr) {
        *intf_ptr = &i2c_addr_list[addr];
        return 0;
    }
    return -1;
}

void i2c_delay_us(uint32_t period, void *intf_ptr) {
    (void)intf_ptr;
    usleep(period);
}

int8_t i2c_open(uint8_t addr) {
    int handle = i2cOpen(1, addr, 0);

    if (handle < 0) {
        printf("Failed to initialise i2c device.\r\n");
        gpioTerminate();
        return COMM_FAIL;
    }
    printf("Initialising with handle %d\r\n", handle);

    handle_list[addr] = handle;
    i2c_addr_list[addr] = addr;

    return 0;
}

int8_t i2c_close(uint8_t addr) {
    if (i2cClose(handle_list[addr]) != 0)
        return COMM_FAIL;
    handle_list[addr] = 0;
    i2c_addr_list[addr] = 0;
    return 0;
}

int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    (void)intf_ptr;
    int handle = handle_list[*(uint8_t *)intf_ptr];
    
    if (i2cReadI2CBlockData(handle, reg_addr, reg_data, len) > 0)
        return 0;
    return COMM_FAIL;
}

int8_t i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    (void)intf_ptr;
    int handle = handle_list[*(uint8_t *)intf_ptr];

    if (i2cWriteI2CBlockData(handle, reg_addr, (uint8_t*)reg_data, len) == 0)
        return 0;
    return COMM_FAIL;
}