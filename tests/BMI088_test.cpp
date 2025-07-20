/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <iostream>
#include <unistd.h>
#include <cassert>
#include "bmi08x.h"

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

int main(void)
{
    int times_to_read = 0;
    Bmi088 bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    Bmi088 bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY);
    bmi08_sensor_data_f data;
    const int ITERATION = 10;

    std::cout << std::endl << "ACCEL DATA" << std::endl
        << "Accel data in m/s^2" << std::endl
        << "Accel data range 24G for BMI088" << std::endl
        << "Sample_Count, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z\n";

    while (times_to_read < ITERATION) {
        if (bmi1.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
            data = bmi1.get_accel_data();

            std::cout << "PRIMARY: " << times_to_read << ", " << data.x << ", " << data.y
                      << ", " << data.z << std::endl;
        }

        /* Assertions for unit testing. Pretty arbitrary numbers. */
//        assert((-1 < data.x) and (data.x < 1));
//        assert((-1 < data.y) and (data.y < 1));
//        assert((8.81 < data.z) and (data.z < 10.81));

        if (bmi2.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
            data = bmi2.get_accel_data();

            std::cout << "SECONDARY: " << times_to_read << ", " << data.x << ", " << data.y
                      << ", " << data.z << std::endl;
        }

        /* Assertions for unit testing. Pretty arbitrary numbers. */
 //       assert((-1 < data.x) and (data.x < 1));
 //       assert((-1 < data.y) and (data.y < 1));
 //       assert((8.81 < data.z) and (data.z < 10.81));

        times_to_read = times_to_read + 1;
        if (times_to_read < ITERATION) usleep(100000);
    }

    times_to_read = 0;

    std::cout << "\n\nGYRO DATA" << std::endl
        << "Gyro data in degrees per second" << std::endl
        << "Gyro data range : 250 dps for BMI088" << std::endl << std::endl
        << "Sample_Count, Gyr_Raw_X, Gyr_Raw_Y, Gyr_Raw_Z, Gyr_DPS_X, Gyr_DPS_Y, Gyr_DPS_Z\n";

    while (times_to_read < ITERATION) {
        if (bmi1.get_gyro_status() & BMI08_GYRO_DATA_READY_INT) {
            data = bmi1.get_gyro_data();

            std::cout << "PRIMARY: " << times_to_read << ", " << data.x << ", " << data.y
                      << ", " << data.z << std::endl;
        }

        /* Assertions for unit testing. Pretty arbitrary numbers. */
//        assert((-5 < data.x) and (data.x < 5));
//        assert((-5 < data.y) and (data.y < 5));
//        assert((-5 < data.z) and (data.z < 5));

        if (bmi2.get_gyro_status() & BMI08_GYRO_DATA_READY_INT) {
            data = bmi2.get_gyro_data();

            std::cout << "SECONDARY: " << times_to_read << ", " << data.x << ", " << data.y
                      << ", " << data.z << std::endl;
        }

        /* Assertions for unit testing. Pretty arbitrary numbers. */
//        assert((-5 < data.x) and (data.x < 5));
//        assert((-5 < data.y) and (data.y < 5));
//        assert((-5 < data.z) and (data.z < 5));

        times_to_read = times_to_read + 1;
        if (times_to_read < ITERATION) usleep(100000);
    }

    return 0;
}
