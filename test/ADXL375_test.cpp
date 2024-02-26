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


int main(void) {
    int8_t rslt;
    uint8_t loop = 0;
    Adxl375 adxl1(ADXL375_ADDR_I2C_PRIM), adxl2(ADXL375_ADDR_I2C_SEC);

    // Initialising the GPIO for I2C communication
    rslt = gpio_initialise();
    printf("Initialised GPIO with return %d\n", rslt);
    if (rslt < 0) {
        gpio_terminate();
        return 1;
    }

    rslt = adxl1.init();
    printf("Initialized adxl375 PRIMARY I2C with return %d\n", rslt);
    if (rslt < 0) {
        adxl1.deinit();
        gpio_terminate();
        return 1;
    }

    rslt = adxl2.init();
    printf("Initialized adxl357 SECONDARY I2C with return %d\n", rslt);
    if (rslt < 0) {
        adxl1.deinit();
        adxl2.deinit();
        gpio_terminate();
        return 1;
    }

    while (loop < ITERATION) {
        rslt = adxl1.get_data();
        if (rslt < 0) {
            adxl1.deinit();
            adxl2.deinit();
            gpio_terminate();
            return 1;
        }

        printf("PRIMARY data[%d] x: %.2f g, y: %.2f g, z: %.2f g\n",
               loop, adxl1.data.x, adxl1.data.y, adxl1.data.z);

        rslt = adxl2.get_data();
        if (rslt < 0) {
            adxl1.deinit();
            adxl2.deinit();
            gpio_terminate();
            return 1;
        }

        printf("SECONDARY data[%d] x: %.2f g, y: %.2f g, z: %.2f g\n",
               loop, adxl2.data.x, adxl2.data.y, adxl2.data.z);

        usleep(1000000);
        loop++;
    }

    adxl1.deinit();
    adxl2.deinit();
    gpio_terminate();

    return 0;
}