/**\
 * Author: Alexis Limozin
 *
 * 
 **/

#include <iostream>
#include <cassert>
#include <unistd.h>

#include "adxl375.h"

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT8_C(3)

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/


int main(void) {
    int loop = 0;
    Adxl375 adxl1(ADXL375_ADDR_I2C_PRIM), adxl2(ADXL375_ADDR_I2C_SEC);    
    adxl375_data data;
    std::cout << "Calibrating..." << std::endl;
    adxl1.calibrate();
    adxl2.calibrate();
    usleep(100000);
    while (loop < ITERATION) {
        if (adxl1.get_status() & ADXL375_DATA_READY) {
            data = adxl1.get_data();
            std::cout << "PRIMARY data[" << loop << "] x: " << data.x << " g, y: "
                      << data.y << " g, z: " << data.z << " g" << std::endl;
        }

        /* data test (when sensor board facing up, so we're supposed to have
        x: 0g, y: 0g, z: 1g) based on typical adxl375 performance (in datasheet)*/
        assert((-0.6 < data.x) and (data.x < 1.4));
        assert((-1.4 < data.y) and (data.y < 0.6));
        assert((-1.2 < data.z) and (data.z < 2.4));

        if (adxl2.get_status() & ADXL375_DATA_READY) {
           data = adxl2.get_data();

            std::cout << "Secondary data[" << loop << "] x: " << data.x << " g, y: "
                      << data.y << " g, z: " << data.z << " g" << std::endl;
        }

        assert((-0.6 < data.x) and (data.x < 1.4));
        assert((-1.4 < data.y) and (data.y < 0.6));
        assert((-1.2 < data.z) and (data.z < 2.4));

        usleep(1000000);
        loop++;
    }

    return 0;
}