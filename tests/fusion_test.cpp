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
#include "kalman.h"
#include "kalman_params.h"
#include "data.h"
/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT16_C(2000)

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/


int main(void) {
    std::ofstream log("/boot/av_log/fusion_test.csv");
    if (!log.is_open()) {
        std::cerr << "Failed to open log file" << std::endl;
        return 1;
    }

    log << "Sample, ADXL_Prim_X, ADXL_Prim_Y, ADXL_Prim_Z, ADXL_Sec_X, ADXL_Sec_Y, ADXL_Sec_Z, "
        << "BMI_Prim_Acc_X, BMI_Prim_Acc_Y, BMI_Prim_Acc_Z, BMI_Prim_Gyr_X, BMI_Prim_Gyr_Y, BMI_Prim_Gyr_Z, "
        << "BMI_Sec_Acc_X, BMI_Sec_Acc_Y, BMI_Sec_Acc_Z, BMI_Sec_Gyr_X, BMI_Sec_Gyr_Y, BMI_Sec_Gyr_Z, "
        << "Azimuth, Pitch" << std::endl;

    int loop = 0;
    try {
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
        bmi08_sensor_data_f dataBM;
        adxl375_data dataAD;

        Kalman kalman(INITIAL_COV_GYR_BIAS,
                      INITIAL_COV_ACCEL_BIAS,
                      INITIAL_COV_ORIENTATION,
                      GYRO_COV,
                      GYRO_BIAS_COV,
                      ACCEL_COV,
                      ACCEL_BIAS_COV,
                      GPS_OBS_COV,
                      ALT_OBS_COV);

        NavSensors nav_sensors;
        NavigationData nav_data;

        nav_data.position.lat = 0;
        nav_data.position.lng = 0;

        usleep(10000);
        while (loop < ITERATION) {
            if (adxl1.get_status() & ADXL375_DATA_READY) {
                dataAD = adxl1.get_data();
                nav_sensors.adxl = dataAD;
            }

            if (adxl2.get_status() & ADXL375_DATA_READY) {
                dataAD = adxl2.get_data();
                nav_sensors.adxl_aux = dataAD;
            }

            if (bmi1.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
                dataBM = bmi1.get_accel_data();
                nav_sensors.bmi_accel = dataBM;
                dataBM = bmi1.get_gyro_data();
                nav_sensors.bmi_gyro = dataBM;
            }

            if (bmi2.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
                dataBM = bmi2.get_accel_data();
                nav_sensors.bmi_aux_accel = dataBM;
                dataBM = bmi2.get_gyro_data();
                nav_sensors.bmi_aux_gyro = dataBM;
            }

            kalman.predict(nav_sensors, nav_data);

            log << loop << ","
                << nav_sensors.adxl.x << "," << nav_sensors.adxl.y << "," << nav_sensors.adxl.z << ","
                << nav_sensors.adxl_aux.x << "," << nav_sensors.adxl_aux.y << "," << nav_sensors.adxl_aux.z << ","
                << nav_sensors.bmi_accel.x << "," << nav_sensors.bmi_accel.y << "," << nav_sensors.bmi_accel.z << ","
                << nav_sensors.bmi_gyro.x << "," << nav_sensors.bmi_gyro.y << "," << nav_sensors.bmi_gyro.z << ","
                << nav_sensors.bmi_aux_accel.x << "," << nav_sensors.bmi_aux_accel.y << "," << nav_sensors.bmi_aux_accel.z << ","
                << nav_sensors.bmi_aux_gyro.x << "," << nav_sensors.bmi_aux_gyro.y << "," << nav_sensors.bmi_aux_gyro.z << ","
                << kalman.get_azimuth() << "," << kalman.get_pitch() << std::endl;

            loop++;
            usleep(10000);
        }
    } catch (const std::exception& e) {
        log << e.what() << std::endl;
        log.close();
        return 1;
    }
    log.close();
    return 0;
}