/**\
 * Author: Alexis Limozin
 *
 * 
 **/

#include <iostream>
#include <cassert>
#include <unistd.h>
#include <fstream>

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

    std::ofstream log("/home/av/logd/adxl375_test.log", std::ios::app);
    if(!log.is_open()){
        log<<"fuckoff" << std::endl;
        log.close();
        return 1;
        
    }




    int loop = 0;
    try
    {
        /* code */
   
    Adxl375 adxl1(ADXL375_ADDR_I2C_PRIM), adxl2(ADXL375_ADDR_I2C_SEC);    
    adxl375_data data;
    log << "Calibrating..." << std::endl;
    adxl1.calibrate();
    adxl2.calibrate();
    usleep(100000);
    while (loop < ITERATION) {
        if (adxl1.get_status() & ADXL375_DATA_READY) {
            data = adxl1.get_data();
            log << "Secondary data[" << loop << "] x: " << data.x << ", y: "
                      << data.y << ", z: " << data.z << std::endl;
        }

        /* data test (when sensor board facing up, so we're supposed to have
        x: 0m/s^2, y: 0m/s^2,, z: 9.81m/s^2) based on typical adxl375 performance (in datasheet)*/
        assert((-5.89< data.x) and (data.x < 13.73));
        assert((-13.73 < data.y) and (data.y < 5.89));
        assert((-11.77 < data.z) and (data.z < 23.5));

        if (adxl2.get_status() & ADXL375_DATA_READY) {
           data = adxl2.get_data();

            log << "Secondary data[" << loop << "] x: " << data.x << ", y: "
                      << data.y << ", z: " << data.z << std::endl;
        }

        // Data test for sensor 2
        assert((-5.89< data.x) and (data.x < 13.73));
        assert((-13.73 < data.y) and (data.y < 5.89));
        assert((-11.77 < data.z) and (data.z < 23.5));

        usleep(1000000);
        loop++;
    }
}
catch(const std::exception& e)
{
   log << e.what() << std::endl;
   log.close();
}
log.close();

    return 0;
}