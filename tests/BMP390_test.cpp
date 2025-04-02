/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <iostream>
#include <unistd.h>
#include <cassert>
#include "bmp3.h"
#include <fstream>
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
    std::ofstream log("~/bmp3_test.log", std::ios::app);
    int loop = 0;
    Bmp390 bmp1(BMP3_ADDR_I2C_PRIM), bmp2(BMP3_ADDR_I2C_SEC);
    bmp3_data data;
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
            data = bmp1.get_sensor_data();

            log << "Data[" << loop << "] PRIMARY T: " << data.temperature
                << " deg C, P: " << data.pressure << " Pa\n";
            
            /* assertions for unit testing. Pretty arbitrary numbers */
            assert((0 < data.temperature) and (data.temperature < 85));
            assert((80000 < data.pressure) and (data.pressure < 120000));
        }

        if (bmp2.get_status().intr.drdy) {
            data = bmp2.get_sensor_data();

            log << "Data[" << loop << "] SECONDARY T: " << data.temperature
                << " deg C, P: " << data.pressure << " Pa\n";

            /* assertions for unit testing. Pretty arbitrary numbers */
            assert((0 < data.temperature) and (data.temperature < 85));
            assert((80000 < data.pressure) and (data.pressure < 120000));
        }

        loop = loop + 1;
        if (loop < ITERATION) sleep(1);
    }
    return 0;
}
