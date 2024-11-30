#include <iostream>
#include <unistd.h>
#include "data.h"
#include "telecom.h"
#include "sensors.h"

int main() {
    // Create drivers
    Adxl375 adxl1(ADXL375_ADDR_I2C_PRIM);
    Adxl375 adxl2(ADXL375_ADDR_I2C_SEC);
    Bmi088 bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    Bmi088 bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY);
    Bmp390 bmp1(BMP3_ADDR_I2C_PRIM);
    Bmp390 bmp2(BMP3_ADDR_I2C_SEC);
    I2CGPS i2cgps;
    TinyGPSPlus gps;

    Sensors sensors(adxl1, adxl2, bmi1, bmi2, bmp1, bmp2, i2cgps, gps);    
    Telecom telecom;

    telecom.begin();
    telecom.send_telemetry();

    //sensors.update();

    usleep(500000);

    telecom.send_telemetry();

    sensors.calibrate();
    //sensors.update();

    usleep(500000);

    telecom.send_telemetry();
}
