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

#include "bmp3.h"
#include "bmi08x.h"

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT8_C(100)

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

int main(void)
{
    int8_t rslt;
    uint8_t loop = 0;
    uint16_t settings_sel;
    struct bmp3_dev dev;
    struct bmp3_data data = { 0 };
    struct bmp3_settings settings = { 0 };
    struct bmp3_status status = { { 0 } };
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

    rslt = bmp3_i2c_init(&dev, BMP3_ADDR_I2C_SEC);
    printf("Initialized bmp3 PRIMARY I2C with return %d\n", rslt);
    
    rslt = bmp3_init(&dev);
    printf("Initialized bmp3 with return %d\n", rslt);
    if (rslt!=0) {
        bmp3_i2c_deinit(BMP3_ADDR_I2C_SEC);
        bmi08_i2c_deinit(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
        gpioTerminate();
        return 1;
    }
    settings.int_settings.drdy_en = BMP3_ENABLE;
    settings.press_en = BMP3_ENABLE;
    settings.temp_en = BMP3_ENABLE;

    settings.odr_filter.press_os = BMP3_OVERSAMPLING_2X;
    settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
    settings.odr_filter.odr = BMP3_ODR_100_HZ;

    settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS | BMP3_SEL_ODR |
                   BMP3_SEL_DRDY_EN;

    rslt = bmp3_set_sensor_settings(settings_sel, &settings, &dev);
    printf("Set settings with return %d\n", rslt);
    if (rslt!=0) {
        bmp3_i2c_deinit(BMP3_ADDR_I2C_SEC);
        bmi08_i2c_deinit(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
        gpioTerminate();
        return 1;
    }
    settings.op_mode = BMP3_MODE_NORMAL;
    rslt = bmp3_set_op_mode(&settings, &dev);
    printf("Set mode with return %d\n", rslt);
    if (rslt!=0) {
        bmp3_i2c_deinit(BMP3_ADDR_I2C_SEC);
        bmi08_i2c_deinit(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
        gpioTerminate();
        return 1;
    }
    
    rslt = bmi08_i2c_init(&bmi08dev, BMI088_VARIANT, BMI08_ACCEL_I2C_ADDR_PRIMARY,
                          BMI08_GYRO_I2C_ADDR_PRIMARY);
    bmi08_error_codes_print_result("bmi08_interface_init", rslt);

    rslt = bmi08_init(&bmi08dev);
    bmi08_error_codes_print_result("init_bmi08", rslt);

    /* Enable data ready interrupts */
    rslt = enable_bmi08_interrupt(&bmi08dev, &accel_int_config, &gyro_int_config);
    bmi08_error_codes_print_result("enable_bmi08_interrupt", rslt);

    if (bmi08dev.accel_cfg.power == BMI08_ACCEL_PM_ACTIVE) {
        printf("\nACCEL DATA\n");
        printf("Accel data in LSB units and Gravity data in m/s^2\n");
        printf("Accel data range : 16G for BMI085 and 24G for BMI088\n\n");

        printf("Sample_Count, Acc_Raw_X, Acc_Raw_Y, Acc_Raw_Z, Acc_ms2_X, Acc_ms2_Y, Acc_ms2_Z\n");

        while (times_to_read < 10) {
            rslt = bmi08a_get_data_int_status(&status_BMI, &bmi08dev);
            bmi08_error_codes_print_result("bmi08a_get_data_int_status", rslt);

            if (status_BMI & BMI08_ACCEL_DATA_READY_INT) {
                rslt = bmi08a_get_data(&bmi08_accel, &bmi08dev);
                bmi08_error_codes_print_result("bmi08a_get_data", rslt);

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
            bmi08_error_codes_print_result("bmi08g_get_data_int_status", rslt);

            if (status_BMI & BMI08_GYRO_DATA_READY_INT) {
                rslt = bmi08g_get_data(&bmi08_gyro, &bmi08dev);
                bmi08_error_codes_print_result("bmi08g_get_data", rslt);

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

    while (loop < ITERATION)
    {
        rslt = bmp3_get_status(&status, &dev);
        printf("rslt : %d\r", rslt);
        if (rslt!=0) {
            bmp3_i2c_deinit(BMP3_ADDR_I2C_SEC);
            bmi08_i2c_deinit(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
            gpioTerminate();
            return 1;
        }
        
        /* Read temperature and pressure data iteratively based on data ready interrupt */
        if ((rslt == BMP3_OK))
        {
            /*
             * First parameter indicates the type of data to be read
             * BMP3_PRESS_TEMP : To read pressure and temperature data
             * BMP3_TEMP       : To read only temperature data
             * BMP3_PRESS      : To read only pressure data
             */
            rslt = bmp3_get_sensor_data(BMP3_PRESS_TEMP, &data, &dev);

            /* NOTE : Read status register again to clear data ready interrupt status */
            rslt = bmp3_get_status(&status, &dev);

            #ifdef BMP3_FLOAT_COMPENSATION
            printf("Data[%d]  T: %.2f deg C, P: %.2f Pa\n", loop, (data.temperature), (data.pressure));
            #else
            printf("Data[%d]  T: %ld deg C, P: %lu Pa\n", loop, (long int)(int32_t)(data.temperature / 100),
                   (long unsigned int)(uint32_t)(data.pressure / 100));
            #endif

            loop = loop + 1;
            sleep(5);
        }
    }

    /* Disable data ready interrupts */
    rslt = disable_bmi08_interrupt(&bmi08dev, &accel_int_config, &gyro_int_config);
    bmi08_error_codes_print_result("disable_bmi08_interrupt", rslt);

    bmp3_i2c_deinit(BMP3_ADDR_I2C_SEC);
    bmi08_i2c_deinit(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    gpioTerminate();
    return rslt;
}
