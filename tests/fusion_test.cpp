/**\
 * Author: Alexis Limozin
 *
 * 
 **/

#include <iostream>
#include <cassert>
#include <unistd.h>
#include "bmi08x.h"
#include <fstream>

#include "adxl375.h"

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT16_C(2000)

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/


int main(void) {

    std::ofstream log("/boot/av_log/fusion_test.log", std::ios::app);
    if(!log.is_open()){
        log<<"fuckoff" << std::endl;
        log.close();
        return 1;
        
    }




    int loop = 0;
    try
    {
        /* code */
        log << "looking for sensors" << std::endl;
        log.flush();
    Adxl375 adxl1(ADXL375_ADDR_I2C_PRIM), adxl2(ADXL375_ADDR_I2C_SEC);   
    log << "adxl found" << std::endl;
    log << "Calibrating..." << std::endl;
    log.flush();
    Bmi088 bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    log << "bmiaccel found" << std::endl;
    log.flush();
    Bmi088 bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY);
    log << "bmigyro found" << std::endl;
    log.flush();
    bmi08_sensor_data_f dataBM;
    adxl375_data dataAD;


    log << std::endl << "ACCEL DATA" << std::endl
        << "Accel data in m/s^2" << std::endl
        << "Accel data range 24G for BMI088" << std::endl
        << "Sample_Count, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z\n";

    //adxl1.calibrate();
    //adxl2.calibrate();
    usleep(10000);
    while (loop < ITERATION) {
        if (adxl1.get_status() & ADXL375_DATA_READY) {
            dataAD = adxl1.get_data();
            log << "Primary data[" << loop << "] x: " << dataAD.x << ", y: "
                      << dataAD.y << ", z: " << dataAD.z << std::endl;
        }

        /* data test (when sensor board facing up, so we're supposed to have
        x: 0m/s^2, y: 0m/s^2,, z: 9.81m/s^2) based on typical adxl375 performance (in datasheet)*/


        if (adxl2.get_status() & ADXL375_DATA_READY) {
        dataAD = adxl2.get_data();
        
        log << "Secondary data[" << loop << "] x: " << dataAD.x << ", y: "
        << dataAD.y << ", z: " << dataAD.z << std::endl;
        }

        if (bmi1.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
            dataBM = bmi1.get_accel_data();

            log << "PRIMARY: " << loop << ", " << dataBM.x << ", " << dataBM.y
                      << ", " << dataBM.z << std::endl;
        }

        if (bmi2.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
            dataBM = bmi2.get_accel_data();

            log << "SECONDARY: " << loop << ", " << dataBM.x << ", " << dataBM.y
                      << ", " << dataBM.z << std::endl;
        }



        loop++;
    }
}
catch(const std::exception& e)
{
   log << e.what() << std::endl;
   log.close();
   return 1;
}
log.close();

    return 0;
}