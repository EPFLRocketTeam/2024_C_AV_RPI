/**\
 * Author: Alexis Limozin
 *
 * 
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>

#include "adxl375.h"
#include "i2c_common.h"

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT8_C(3)

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

static int error_return() {
    adxl375_i2c_deinit(ADXL375_ADDR_I2C_PRIM);
    adxl375_i2c_deinit(ADXL375_ADDR_I2C_SEC);
    gpio_terminate();
    return 1;
}

int main(void) {
    int8_t rslt;
    uint8_t loop = 0;
    uint16_t settings_sel;
    struct adxl375_dev dev1, dev2;
    int16_t x1, y1, z1, x2, y2, z2;

    // Initialising the GPIO for I2C communication
    rslt = gpio_initialise();
    printf("Initialised GPIO with return %d\n", rslt);
    if (rslt < 0) {
        gpio_terminate();
        return 1;
    }

    rslt = adxl375_init(&dev1, ADXL375_ADDR_I2C_PRIM);
    usleep(100000);
    printf("Initialized adxl375 PRIMARY I2C with return %d\n", rslt);
    if (rslt < 0) {
        adxl375_i2c_deinit(ADXL375_ADDR_I2C_PRIM);
        gpio_terminate();
        return 1;
    }

    rslt = adxl375_init(&dev2, ADXL375_ADDR_I2C_SEC);
    usleep(100000);
    printf("Initialized adxl357 SECONDARY I2C with return %d\n", rslt);
    if (rslt!=0) return error_return();

    while (loop < ITERATION) {
        rslt = adxl375_get_xyz(&dev1, &x1, &y1, &z1);
        if (rslt!=0) return error_return();

        printf("PRIMARY data[%d] x: %d, y: %d, z: %d\n",
               loop, x1, y1, z1);

        rslt = adxl375_get_xyz(&dev2, &x2, &y2, &z2);
        if (rslt!=0) return error_return();

        printf("SECONDARY data[%d] x: %d, y: %d, z: %d\n",
               loop, x2, y2, z2);
        
        usleep(1000000);
        loop++;
    }

    adxl375_i2c_deinit(ADXL375_ADDR_I2C_PRIM);
    adxl375_i2c_deinit(ADXL375_ADDR_I2C_SEC);
    gpio_terminate();

    return 0;
}