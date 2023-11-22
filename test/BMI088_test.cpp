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
#include <pigpio.h>

#include "bmi08x.h"

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

static int error_return(const char api_name[], int8_t rslt) {
    bmi08_error_codes_print_result(api_name, rslt);
    bmi08_i2c_deinit(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    gpioTerminate();
    return 1;
}

int main(void)
{
    int8_t rslt;
    uint16_t settings_sel;
    struct bmi08_dev bmi08dev;

    uint8_t times_to_read = 0;
    struct bmi08_sensor_data bmi08_accel;
    struct bmi08_sensor_data bmi08_gyro;
    struct bmi08_accel_int_channel_cfg accel_int_config;
    struct bmi08_gyro_int_channel_cfg gyro_int_config;
    float x = 0.0, y = 0.0, z = 0.0;
    uint8_t status_BMI = 0;

    /* Interface reference is given as a parameter
     *         For I2C : BMP3_I2C_INTF
     *         For SPI : BMP3_SPI_INTF
     */
    rslt = gpioInitialise();
    printf("Initialised GPIO with return %d\n", rslt);

    rslt = bmi08_i2c_init(&bmi08dev, BMI088_VARIANT, BMI08_ACCEL_I2C_ADDR_PRIMARY,
                          BMI08_GYRO_I2C_ADDR_PRIMARY);
    if (rslt != 0) return error_return("bmi08_interface_init", rslt);

    rslt = bmi08_init(&bmi08dev);
    if (rslt != 0) return error_return("init_bmi08", rslt);

    /* Enable data ready interrupts */
    rslt = enable_bmi08_interrupt(&bmi08dev, &accel_int_config, &gyro_int_config);
    if (rslt != 0) return error_return("enable_bmi08_interrupt", rslt);

    if (bmi08dev.accel_cfg.power == BMI08_ACCEL_PM_ACTIVE) {
        printf("\nACCEL DATA\n");
        printf("Accel data in LSB units and Gravity data in m/s^2\n");
        printf("Accel data range : 16G for BMI085 and 24G for BMI088\n\n");

        printf("Sample_Count, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z\n");

        while (times_to_read < 10) {
            rslt = bmi08a_get_data_int_status(&status_BMI, &bmi08dev);
            if (rslt != 0) return error_return("bmi08a_get_data_int_status", rslt);

            if (status_BMI & BMI08_ACCEL_DATA_READY_INT) {
                rslt = bmi08a_get_data(&bmi08_accel, &bmi08dev);
                if (rslt != 0) return error_return("bmi08a_get_data", rslt);

                if (bmi08dev.variant == BMI085_VARIANT) {
                    /* Converting lsb to meter per second squared for 16 bit accelerometer at 16G range. */
                    x = lsb_to_mps2(bmi08_accel.x, 16, 16);
                    y = lsb_to_mps2(bmi08_accel.y, 16, 16);
                    z = lsb_to_mps2(bmi08_accel.z, 16, 16);
                }
                else if (bmi08dev.variant == BMI088_VARIANT) {
                    /* Converting lsb to meter per second squared for 16 bit accelerometer at 24G range. */
                    x = lsb_to_mps2(bmi08_accel.x, 24, 16);
                    y = lsb_to_mps2(bmi08_accel.y, 24, 16);
                    z = lsb_to_mps2(bmi08_accel.z, 24, 16);
                }

                printf("%d, %5d, %5d, %5d, %4.2f, %4.2f, %4.2f\n",
                        times_to_read,
                        bmi08_accel.x,
                        bmi08_accel.y,
                        bmi08_accel.z,
                        x,
                        y,
                        z);

                times_to_read = times_to_read + 1;
            }
        }
    }
    if (bmi08dev.gyro_cfg.power == BMI08_GYRO_PM_NORMAL) {
        times_to_read = 0;

        printf("\n\nGYRO DATA\n");
        printf("Gyro data in LSB units and degrees per second\n");
        printf("Gyro data range : 250 dps for BMI085 and BMI088\n\n");

        printf("Sample_Count, Gyr_Raw_X, Gyr_Raw_Y, Gyr_Raw_Z, Gyr_DPS_X, Gyr_DPS_Y, Gyr_DPS_Z\n");

        while (times_to_read < 10) {
            rslt = bmi08g_get_data_int_status(&status_BMI, &bmi08dev);
            if (rslt != 0) return error_return("bmi08g_get_data_int_status", rslt);

            if (status_BMI & BMI08_GYRO_DATA_READY_INT) {
                rslt = bmi08g_get_data(&bmi08_gyro, &bmi08dev);
                if (rslt != 0) return error_return("bmi08g_get_data", rslt);

                /* Converting lsb to degree per second for 16 bit gyro at 250 dps range. */
                x = lsb_to_dps(bmi08_gyro.x, (float)250, 16);
                y = lsb_to_dps(bmi08_gyro.y, (float)250, 16);
                z = lsb_to_dps(bmi08_gyro.z, (float)250, 16);

                printf("%d, %5d, %5d, %5d, %4.2f, %4.2f, %4.2f\n",
                        times_to_read,
                        bmi08_gyro.x,
                        bmi08_gyro.y,
                        bmi08_gyro.z,
                        x,
                        y,
                        z);

                times_to_read = times_to_read + 1;
            }
        }
    }

    /* Disable data ready interrupts */
    rslt = disable_bmi08_interrupt(&bmi08dev, &accel_int_config, &gyro_int_config);
    if (rslt != 0) return error_return("disable_bmi08_interrupt", rslt);
    bmi08_i2c_deinit(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    gpioTerminate();
    return rslt;
}
