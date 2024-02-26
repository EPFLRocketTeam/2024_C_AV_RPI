/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>

#include "bmp3.h"
#include "i2c_common.h"

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT8_C(3)

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

int main(void)
{
    int8_t rslt;
    uint8_t loop = 0;
    Bmp390 bmp1(BMP3_ADDR_I2C_PRIM), bmp2(BMP3_ADDR_I2C_SEC);

    // Initialising the GPIO for I2C communication
    rslt = gpio_initialise();
    printf("Initialised GPIO with return %d\n", rslt);
    if (rslt < 0) {
        gpio_terminate();
        return 1;
    }

    // Initialising the I2C channels
    rslt = bmp1.init();
    if (rslt!=0) {
        bmp1.deinit();
        gpio_terminate();
        return 1;
    }

    rslt = bmp2.init();
    if (rslt!=0) {
        bmp1.deinit();
        bmp2.deinit();
        gpio_terminate();
        return 1;
    }

    // Main get data loop
    while (loop < ITERATION)
    {
        rslt = bmp1.get_status();
        if (rslt!=0) {
            bmp1.deinit();
            bmp2.deinit();
            gpio_terminate();
            return 1;
        }

        rslt = bmp2.get_status();
        if (rslt!=0) {
            bmp1.deinit();
            bmp2.deinit();
            gpio_terminate();
            return 1;
        }
        
        /* Read temperature and pressure data iteratively based on data ready interrupt */
        /*
            * First parameter indicates the type of data to be read
            * BMP3_PRESS_TEMP : To read pressure and temperature data
            * BMP3_TEMP       : To read only temperature data
            * BMP3_PRESS      : To read only pressure data
            */
        rslt = bmp1.get_sensor_data();

        #ifdef BMP3_FLOAT_COMPENSATION
        printf("Data[%d] PRIMARY T: %.2f deg C, P: %.2f Pa\n", loop, (bmp1.data.temperature), (bmp1.data.pressure));
        #else
        printf("Data[%d]  T: %ld deg C, P: %lu Pa\n", loop, (long int)(int32_t)(data1.temperature / 100),
                (long unsigned int)(uint32_t)(data1.pressure / 100));
        #endif
        rslt = bmp1.test_data();
        if (rslt!=0) return rslt;


        rslt = bmp2.get_sensor_data();

        #ifdef BMP3_FLOAT_COMPENSATION
        printf("Data[%d] SECONDARY T: %.2f deg C, P: %.2f Pa\n", loop, (bmp2.data.temperature), (bmp2.data.pressure));
        #else
        printf("Data[%d]  T: %ld deg C, P: %lu Pa\n", loop, (long int)(int32_t)(data2.temperature / 100),
                (long unsigned int)(uint32_t)(data2.pressure / 100));
        #endif
        rslt = bmp2.test_data();
        if (rslt!=0) return rslt;

        loop = loop + 1;
        if (loop < ITERATION) sleep(1);
    }

    bmp1.deinit();
    bmp2.deinit();
    gpio_terminate();

    return 0;
}
