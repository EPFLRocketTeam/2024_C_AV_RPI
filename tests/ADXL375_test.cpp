/**\
 * Author: Alexis Limozin
 *
 * 
 **/

#include <iostream>

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

    while (loop < ITERATION) {
        if (adxl1.get_status() & ADXL375_DATA_READY) {
            adxl375_data data1 = adxl1.get_data();
            std::cout << "PRIMARY data[" << loop << "] x: " << data1.x << " g, y: "
                      << data1.y << " g, z: " << data1.z << " g" << std::endl;
        }

        if (adxl2.get_status() & ADXL375_DATA_READY) {
           adxl375_data data2 = adxl2.get_data();

            std::cout << "Secondary data[" << loop << "] x: " << data2.x << " g, y: "
                      << data2.y << " g, z: " << data2.z << " g" << std::endl;
        }

        usleep(1000000);
        loop++;
    }

    return 0;
}