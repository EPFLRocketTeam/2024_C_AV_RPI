/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <iostream>

#include "bmi08x.h"

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

int main(void)
{
    int times_to_read = 0;
    Bmi088 bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    Bmi088 bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY);


    std::cout << std::endl << "ACCEL DATA" << std::endl
        << "Accel data in m/s^2" << std::endl
        << "Accel data range 24G for BMI088" << std::endl
        << "Sample_Count, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z" << std::endl;

    while (times_to_read < 10) {
        if (bmi1.get_status() & BMI08_ACCEL_DATA_READY_INT) {
            bmi08_sensor_data_f data1 = bmi1.get_accel_data();

            std::cout << "PRIMARY: " << times_to_read << ", " << data1.x << ", " << data1.y
                      << ", " << data1.z << std::endl;
        }

        if (bmi2.get_status() & BMI08_ACCEL_DATA_READY_INT) {
            bmi08_sensor_data_f data2 = bmi2.get_accel_data();

            std::cout << "SECONDARY: " << times_to_read << ", " << data2.x << ", " << data2.y
                      << ", " << data2.z << std::endl;
        }
        times_to_read = times_to_read + 1;
    }

    times_to_read = 0;

    std::cout << "\n\nGYRO DATA" << std::endl
        << "Gyro data in degrees per second" << std::endl
        << "Gyro data range : 250 dps for BMI088" << std::endl << std::endl
        << "Sample_Count, Gyr_Raw_X, Gyr_Raw_Y, Gyr_Raw_Z, Gyr_DPS_X, Gyr_DPS_Y, Gyr_DPS_Z" << std::endl;

    while (times_to_read < 10) {
        if (bmi1.get_status() & BMI08_GYRO_DATA_READY_INT) {
            bmi08_sensor_data_f data1 = bmi1.get_gyro_data();

            std::cout << "PRIMARY: " << times_to_read << ", " << data1.x << ", " << data1.y
                      << ", " << data1.z << std::endl;
        }

        if (bmi2.get_status() & BMI08_GYRO_DATA_READY_INT) {
            bmi08_sensor_data_f data2 = bmi2.get_gyro_data();

            std::cout << "SECONDARY: " << times_to_read << ", " << data2.x << ", " << data2.y
                      << ", " << data2.z << std::endl;
        }
        times_to_read = times_to_read + 1;
    }

    return 0;
}
