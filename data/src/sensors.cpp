#include "sensors.h"
#include "bmi08.h"
#include "bmp3_defs.h"
#include "adxl375.h"
#include "TinyGPS++.h"
#include "I2CGPS.h"


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
//TODO Where to import te defs (2 sources) ?
Sensors::Sensors(Data data)
:   adxl1(ADXL375_ADDR_I2C_PRIM),
    adxl2(ADXL375_ADDR_I2C_SEC),
    bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY),
    bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY),
    bmp1(BMP3_ADDR_I2C_PRIM),
    bmp2(BMP3_ADDR_I2C_SEC),
    i2cgps(),
    gps()
{
    this->data = data;
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
    this->data.goat.sensors_raw.adxl = adxl1.get_data();
    this->data.goat.sensors_raw.adxl_aux = adxl2.get_data();
    this->data.goat.sensors_raw.bmi_accel = bmi1.get_accel_data();
    this->data.goat.sensors_raw.bmi_gyro = bmi1.get_gyro_data();
    this->data.goat.sensors_raw.bmi_aux_accel = bmi2.get_accel_data();
    this->data.goat.sensors_raw.bmi_aux_gyro = bmi2.get_gyro_data();
    this->data.goat.sensors_raw.bmp = bmp1.get_sensor_data();
    this->data.goat.sensors_raw.bmp_aux = bmp2.get_sensor_data();
    // Call Kalmann filter API
    // Something like: this->data.goat.sensors_data = Kalmann(SensRaw raw_data); ?
    
    while (i2cgps.available()) {
        gps.encode(i2cgps.read());
    }
    if (gps.time.isUpdated()) {
        if (gps.date.isValid()) {
            this->data.goat.sensors_data.time.year = static_cast<int>(gps.date.year());
            this->data.goat.sensors_data.time.month = static_cast<int>(gps.date.month());
            this->data.goat.sensors_data.time.day = static_cast<int>(gps.date.day());
        }
        if (gps.time.isValid()) {
            this->data.goat.sensors_data.time.hour = static_cast<int>(gps.time.hour());
            this->data.goat.sensors_data.time.minute = static_cast<int>(gps.time.minute());
            this->data.goat.sensors_data.time.second = static_cast<int>(gps.time.second());
            this->data.goat.sensors_data.time.centisecond = static_cast<int>(gps.time.centisecond());
        }
        if (gps.location.isValid()) {
            this->data.goat.sensors_data.position.lat = gps.location.lat();
            this->data.goat.sensors_data.position.lng = gps.location.lng();
        }
        if (gps.altitude.isValid()) {
            this->data.goat.sensors_data.position.alt = gps.altitude.meters();
        }
        if (gps.course.isValid()) {
            this->data.goat.sensors_data.course = gps.course.deg();
        }
    }
    return true;
}

void Sensors::update_status() {
    this->data.goat.sensors_status.adxl_status = adxl1.get_status();
    this->data.goat.sensors_status.adxl_aux_status = adxl2.get_status();
    this->data.goat.sensors_status.bmi_accel_status = bmi1.get_accel_status();
    this->data.goat.sensors_status.bmi_aux_accel_status = bmi2.get_accel_status();
    this->data.goat.sensors_status.bmi_gyro_status = bmi1.get_gyro_status();
    this->data.goat.sensors_status.bmi_aux_gyro_status = bmi2.get_gyro_status();
    this->data.goat.sensors_status.bmp_status = bmp1.get_status();
    this->data.goat.sensors_status.bmp_aux_status = bmp2.get_status();
}