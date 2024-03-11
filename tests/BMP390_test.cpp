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
    int loop = 0;
    Bmp390 bmp1(BMP3_ADDR_I2C_PRIM), bmp2(BMP3_ADDR_I2C_SEC);

    // Main get data loop
    while (loop < ITERATION)
    {
        /* Read temperature and pressure data iteratively based on data ready interrupt */
        /*
            * First parameter indicates the type of data to be read
            * BMP3_PRESS_TEMP : To read pressure and temperature data
            * BMP3_TEMP       : To read only temperature data
            * BMP3_PRESS      : To read only pressure data
            */
        if (bmp1.get_status().intr.drdy) {
            bmp3_data data1 = bmp1.get_sensor_data();

            printf("Data[%d] PRIMARY T: %.2f deg C, P: %.2f Pa\n", loop, (data1.temperature), (data1.pressure));

            // test data

            
        }

        if (bmp2.get_status().intr.drdy) {
            bmp3_data data2 = bmp2.get_sensor_data();

            printf("Data[%d] SECONDARY T: %.2f deg C, P: %.2f Pa\n", loop, (data2.temperature), (data2.pressure));

            // test data

            
        }

        loop = loop + 1;
        if (loop < ITERATION) sleep(1);
    }
    return 0;
}
