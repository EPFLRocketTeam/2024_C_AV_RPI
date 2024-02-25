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

#include "bmp3.h"
#include "i2c_common.h"

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT8_C(3)

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/

static int error_return() {
    bmp3_i2c_deinit(BMP3_ADDR_I2C_PRIM);
    bmp3_i2c_deinit(BMP3_ADDR_I2C_SEC);
    gpio_terminate();
    return 1;
}

static void initialize_settings(struct bmp3_settings *settings) {
    settings->int_settings.drdy_en = BMP3_ENABLE;
    settings->press_en = BMP3_ENABLE;
    settings->temp_en = BMP3_ENABLE;

    settings->odr_filter.press_os = BMP3_OVERSAMPLING_2X;
    settings->odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
    settings->odr_filter.odr = BMP3_ODR_100_HZ;

    settings->op_mode = BMP3_MODE_NORMAL;
}

/* 
 * Testing the data with arbitrary numbers in degrees celsius and pascals.
 */
static int test_data(struct bmp3_data data) {
    if (data.temperature > 50 || data.temperature < 5
        || data.pressure > 120000 || data.pressure < 80000) {
            return 1;
    }
    return 0;
}

int main(void)
{
    int8_t rslt;
    uint8_t loop = 0;
    uint16_t settings_sel;
    struct bmp3_dev dev1, dev2;
    struct bmp3_data data1 = { 0 }, data2 = { 0 };
    struct bmp3_settings settings1 = { 0 }, settings2 = { 0 };
    struct bmp3_status status1 = { { 0 } }, status2 = { { 0 } };

    // Initialising the GPIO for I2C communication
    rslt = gpio_initialise();
    printf("Initialised GPIO with return %d\n", rslt);
    if (rslt < 0) {
        gpio_terminate();
        return 1;
    }

    // Initialising the I2C channels
    rslt = bmp3_i2c_init(&dev1, BMP3_ADDR_I2C_PRIM);
    printf("Initialized bmp3 PRIMARY I2C with return %d\n", rslt);
    if (rslt!=0) {
        bmp3_i2c_deinit(BMP3_ADDR_I2C_PRIM);
        gpio_terminate();
        return 1;
    }

    rslt = bmp3_i2c_init(&dev2, BMP3_ADDR_I2C_SEC);
    printf("Initialized bmp3 SECONDARY I2C with return %d\n", rslt);
    if (rslt!=0) return error_return();

    //Initialising the devices
    rslt = bmp3_init(&dev1);
    printf("Initialized bmp3 PRIMARY with return %d\n", rslt);
    if (rslt!=0) return error_return();

    rslt = bmp3_init(&dev2);
    printf("Initialized bmp3 SECONDARY with return %d\n", rslt);
    if (rslt!=0) return error_return();

    initialize_settings(&settings1);
    initialize_settings(&settings2);

    settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_PRESS_OS |
                   BMP3_SEL_TEMP_OS | BMP3_SEL_ODR | BMP3_SEL_DRDY_EN;

    rslt = bmp3_set_sensor_settings(settings_sel, &settings1, &dev1);
    printf("Set settings on PRIMARY with return %d\n", rslt);
    if (rslt!=0) return error_return();

    rslt = bmp3_set_sensor_settings(settings_sel, &settings2, &dev2);
    printf("Set settings on SECONDARY with return %d\n", rslt);
    if (rslt!=0) return error_return();

    rslt = bmp3_set_op_mode(&settings1, &dev1);
    printf("Set mode on PRIMARY with return %d\n", rslt);
    if (rslt!=0) return error_return();

    rslt = bmp3_set_op_mode(&settings2, &dev2);
    printf("Set mode on SECONDARY with return %d\n", rslt);
    if (rslt!=0) return error_return();

    // Main get data loop
    while (loop < ITERATION)
    {
        rslt = bmp3_get_status(&status1, &dev1);
        printf("rslt PRIMARY : %d\r", rslt);
        if (rslt!=0) return error_return();
        rslt = bmp3_get_status(&status2, &dev2);
        printf("rslt SECONDARY : %d\r", rslt);
        if (rslt!=0) return error_return();
        
        /* Read temperature and pressure data iteratively based on data ready interrupt */
        /*
            * First parameter indicates the type of data to be read
            * BMP3_PRESS_TEMP : To read pressure and temperature data
            * BMP3_TEMP       : To read only temperature data
            * BMP3_PRESS      : To read only pressure data
            */
        rslt = bmp3_get_sensor_data(BMP3_PRESS_TEMP, &data1, &dev1);

        /* NOTE : Read status register again to clear data ready interrupt status */
        rslt = bmp3_get_status(&status1, &dev1);

        #ifdef BMP3_FLOAT_COMPENSATION
        printf("Data[%d]  T: %.2f deg C, P: %.2f Pa\n", loop, (data1.temperature), (data1.pressure));
        #else
        printf("Data[%d]  T: %ld deg C, P: %lu Pa\n", loop, (long int)(int32_t)(data1.temperature / 100),
                (long unsigned int)(uint32_t)(data1.pressure / 100));
        #endif
        rslt = test_data(data1);
        if (rslt!=0) return rslt;

        rslt = bmp3_get_sensor_data(BMP3_PRESS_TEMP, &data2, &dev2);

        /* NOTE : Read status register again to clear data ready interrupt status */
        rslt = bmp3_get_status(&status1, &dev2);

        #ifdef BMP3_FLOAT_COMPENSATION
        printf("Data[%d]  T: %.2f deg C, P: %.2f Pa\n", loop, (data2.temperature), (data2.pressure));
        #else
        printf("Data[%d]  T: %ld deg C, P: %lu Pa\n", loop, (long int)(int32_t)(data2.temperature / 100),
                (long unsigned int)(uint32_t)(data2.pressure / 100));
        #endif
        rslt = test_data(data2);
        if (rslt!=0) return rslt;

        loop = loop + 1;
        if (loop < ITERATION) sleep(1);
    }

    bmp3_i2c_deinit(BMP3_ADDR_I2C_PRIM);
    bmp3_i2c_deinit(BMP3_ADDR_I2C_SEC);
    gpio_terminate();

    return 0;
}
