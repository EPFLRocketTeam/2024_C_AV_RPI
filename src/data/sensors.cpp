#include "sensors.h"

SensRaw::SensRaw()
:   adxl{0, 0, 0},
    adxl_aux{0, 0, 0},
    bmi_accel{0, 0, 0},
    bmi_gyro{0, 0, 0},
    bmi_aux_accel{0, 0, 0},
    bmi_aux_gyro{0, 0, 0},
    bmp{0, 0},
    bmp_aux{0, 0}
{}

SensFiltered::SensFiltered()
:   position{0, 0, 0},
    speed{0, 0, 0},
    accel{0, 0, 0},
    attitude{0, 0, 0},
    course(0),
    altitude(0),
    baro{0, 0},
    N2_pressure(0),
    fuel_pressure(0),
    LOX_pressure(0),
    fuel_level(0),
    LOX_level(0),
    engine_temperature(0),
    igniter_pressure(0),
    LOX_inj_pressure(0),
    fuel_inj_pressure(0),
    chamber_pressure(0)
{}

Sensors::Sensors() 
:   adxl1(ADXL375_ADDR_I2C_PRIM),
    adxl2(ADXL375_ADDR_I2C_SEC),
    bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY),
    bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY),
    bmp1(BMP3_ADDR_I2C_PRIM),
    bmp2(BMP3_ADDR_I2C_SEC),
    i2cgps(),
    gps() 
{
    update_status();
}

Sensors::~Sensors() {}

void Sensors::calibrate() {
    adxl1.calibrate();
    adxl2.calibrate();
}

bool Sensors::update() {
    update_status();

    // Update raw sensors values
    raw_data.adxl = adxl1.get_data();
    raw_data.adxl_aux = adxl2.get_data();

    raw_data.bmi_accel = bmi1.get_accel_data();
    raw_data.bmi_gyro = bmi1.get_gyro_data();
    raw_data.bmi_aux_accel = bmi2.get_accel_data();
    raw_data.bmi_aux_gyro = bmi2.get_gyro_data();

    raw_data.bmp = bmp1.get_sensor_data();
    raw_data.bmp_aux = bmp2.get_sensor_data();

    // Call Kalmann filter API
    // Something like: clean_data = Kalmann(SensRaw raw_data); ?
    
    while (i2cgps.available()) {
        gps.encode(i2cgps.read());
    }
    if (gps.time.isUpdated()) {
        if (gps.date.isValid()) {
            clean_data.time.year = static_cast<int>(gps.date.year());
            clean_data.time.month = static_cast<int>(gps.date.month());
            clean_data.time.day = static_cast<int>(gps.date.day());
        }
        if (gps.time.isValid()) {
            clean_data.time.hour = static_cast<int>(gps.time.hour());
            clean_data.time.minute = static_cast<int>(gps.time.minute());
            clean_data.time.second = static_cast<int>(gps.time.second());
            clean_data.time.centisecond = static_cast<int>(gps.time.centisecond());
        }
        if (gps.location.isValid()) {
            clean_data.position.lat = gps.location.lat();
            clean_data.position.lng = gps.location.lng();
        }
        if (gps.altitude.isValid()) {
            clean_data.position.alt = gps.altitude.meters();
        }
        if (gps.course.isValid()) {
            clean_data.course = gps.course.deg();
        }
    }
    return true;
}

void Sensors::update_status() {
    status.adxl_status = adxl1.get_status();
    status.adxl_aux_status = adxl2.get_status();
    status.bmi_accel_status = bmi1.get_accel_status();
    status.bmi_aux_accel_status = bmi2.get_accel_status();
    status.bmi_gyro_status = bmi1.get_gyro_status();
    status.bmi_aux_gyro_status = bmi2.get_gyro_status();
    status.bmp_status = bmp1.get_status();
    status.bmp_aux_status = bmp2.get_status();
}