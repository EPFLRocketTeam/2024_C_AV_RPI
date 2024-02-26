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

#include "bmi08x.h"
#include "i2c_common.h"

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

int main(void)
{
    int8_t rslt;
    uint8_t times_to_read = 0;
    Bmi088 bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    Bmi088 bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY);

    // Initialising gpio interface
    rslt = gpio_initialise();
    printf("Initialised GPIO with return %d\n", rslt);
    if (rslt < 0) {
        gpio_terminate();
        return 1;
    }

    // Initialising i2c interface for both devices
    rslt = bmi1.init();
    if (rslt != 0) {
        bmi1.deinit();
        gpio_terminate();
        return 1;
    }

    rslt = bmi2.init();
    if (rslt != 0) {
        bmi1.deinit();
        bmi2.deinit();
        gpio_terminate();
        return 1;
    }


    // // Soft resetting both devices (necessary) to start from a known state
    // bmi08a_soft_reset(&dev1);
    // bmi08a_soft_reset(&dev2);
    // // Delay for the soft reset 
    // usleep(10000);

    printf("\nACCEL DATA\n");
    printf("Accel data in m/s^2\n");
    printf("Accel data range 24G for BMI088\n\n");

    printf("Sample_Count, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z\n");

    while (times_to_read < 10) {
        rslt = bmi1.get_status();
        if (rslt != 0) {
            bmi1.deinit();
            bmi2.deinit();
            gpio_terminate();
            return 1;
        }

        if (bmi1.status & BMI08_ACCEL_DATA_READY_INT) {
            rslt = bmi1.get_accel_data();
            if (rslt != 0) {
                bmi1.deinit();
                bmi2.deinit();
                gpio_terminate();
                return 1;
            }

            printf("PRIMARY: %d, %4.2f, %4.2f, %4.2f\n",
                    times_to_read,
                    bmi1.accel_data.x, bmi1.accel_data.y, bmi1.accel_data.z);
        }

        rslt = bmi2.get_status();
        if (rslt != 0) {
            bmi1.deinit();
            bmi2.deinit();
            gpio_terminate();
            return 1;
        }

        if (bmi2.status & BMI08_ACCEL_DATA_READY_INT) {
            rslt = bmi2.get_accel_data();
            if (rslt != 0) {
                bmi1.deinit();
                bmi2.deinit();
                gpio_terminate();
                return 1;
            }

            printf("SECONDARY: %d, %4.2f, %4.2f, %4.2f\n",
                    times_to_read,
                    bmi2.accel_data.x, bmi2.accel_data.y, bmi2.accel_data.z);
        }
        times_to_read = times_to_read + 1;
    }

    times_to_read = 0;

    printf("\n\nGYRO DATA\n");
    printf("Gyro data in degrees per second\n");
    printf("Gyro data range : 250 dps for BMI088\n\n");

    printf("Sample_Count, Gyr_Raw_X, Gyr_Raw_Y, Gyr_Raw_Z, Gyr_DPS_X, Gyr_DPS_Y, Gyr_DPS_Z\n");

    while (times_to_read < 10) {
        rslt = bmi1.get_status();
        if (rslt != 0) {
            bmi1.deinit();
            bmi2.deinit();
            gpio_terminate();
            return 1;
        }

        if (bmi1.status & BMI08_GYRO_DATA_READY_INT) {
            rslt = bmi1.get_gyro_data();
            if (rslt != 0) {
                bmi1.deinit();
                bmi2.deinit();
                gpio_terminate();
                return 1;
            }

            printf("PRIMARY: %d, %4.2f, %4.2f, %4.2f\n",
                    times_to_read,
                    bmi1.gyro_data.x, bmi1.gyro_data.y, bmi1.gyro_data.z);
        }

        rslt = bmi2.get_status();
        if (rslt != 0) {
            bmi1.deinit();
            bmi2.deinit();
            gpio_terminate();
            return 1;
        }

        if (bmi2.status & BMI08_GYRO_DATA_READY_INT) {
            rslt = bmi2.get_gyro_data();
            if (rslt != 0) {
                bmi1.deinit();
                bmi2.deinit();
                gpio_terminate();
                return 1;
            }

            printf("SECONDARY: %d, %4.2f, %4.2f, %4.2f\n",
                    times_to_read,
                    bmi2.gyro_data.x, bmi2.gyro_data.y, bmi2.gyro_data.z);
        }
        times_to_read = times_to_read + 1;
    }

    /* Disable data ready interrupts */
    bmi1.deinit();
    bmi2.deinit();
    gpio_terminate();
    return rslt;
}
