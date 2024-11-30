#include "sensors.h"
#include "data.h"
#include <iostream>
Sensors::Sensors(Adxl375_Interface& adxl1_ref, Adxl375_Interface& adxl2_ref,
                 Bmi088_Interface& bmi1_ref, Bmi088_Interface& bmi2_ref,
                 Bmp3_Interface& bmp1_ref, Bmp3_Interface& bmp2_ref,
                 I2CGPS_Interface& i2cgps_ref, TinyGPSPlus_Interface& gps_ref)
    : adxl1(adxl1_ref),
      adxl2(adxl2_ref),
      bmi1(bmi1_ref),
      bmi2(bmi2_ref),
      bmp1(bmp1_ref),
      bmp2(bmp2_ref),
      i2cgps(i2cgps_ref),
      gps(gps_ref)
{
    //update_status();
}

Sensors::~Sensors() {}

void Sensors::calibrate() {
    adxl1.calibrate();
    adxl2.calibrate();
}

double Vector3::norm() const {
    return std::sqrt(x * x + y * y + z * z);
}

bool Sensors::update() {
    update_status();

    // Update raw sensors values and write them to the GOAT
    auto temp_adxl(adxl1.get_data());
    Data::get_instance().write(Data::NAV_SENSOR_ADXL1_DATA, &temp_adxl);
    temp_adxl = adxl2.get_data();
    Data::get_instance().write(Data::NAV_SENSOR_ADXL2_DATA, &temp_adxl);
    auto temp_bmi(bmi1.get_accel_data());
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_ACCEL_DATA, &temp_bmi);
    temp_bmi = bmi1.get_gyro_data();
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_GYRO_DATA, &temp_bmi);
    temp_bmi = bmi2.get_accel_data();
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_ACCEL_DATA, &temp_bmi);
    temp_bmi = bmi2.get_gyro_data();
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_GYRO_DATA, &temp_bmi);
    auto temp_bmp(bmp1.get_sensor_data());
    Data::get_instance().write(Data::NAV_SENSOR_BMP1_DATA, &temp_bmp);
    temp_bmp = bmp2.get_sensor_data();
    Data::get_instance().write(Data::NAV_SENSOR_BMP2_DATA, &temp_bmp);

    // TODO: Propulsion sensors acquisition
    
    while (i2cgps.available()) {
        gps.encode(i2cgps.read());
    }
    if (gps.getTime().isUpdated()) {
        if (gps.date.isValid()) {
            unsigned temp(gps.date.year());
            Data::get_instance().write(Data::NAV_GNSS_TIME_YEAR, &temp);
            temp = gps.date.month();
            Data::get_instance().write(Data::NAV_GNSS_TIME_MONTH, &temp);
            temp = gps.date.day();
            Data::get_instance().write(Data::NAV_GNSS_TIME_DAY, &temp);
        }
        if (gps.time.isValid()) {
            unsigned temp(gps.time.hour());
            Data::get_instance().write(Data::NAV_GNSS_TIME_HOUR, &temp);
            temp = gps.time.minute();
            Data::get_instance().write(Data::NAV_GNSS_TIME_MINUTE, &temp);
            temp = gps.time.second();
            Data::get_instance().write(Data::NAV_GNSS_TIME_SECOND, &temp);
            temp = gps.time.centisecond();
            Data::get_instance().write(Data::NAV_GNSS_TIME_CENTI, &temp);
        }
        if (gps.location.isValid()) {
            double temp(gps.location.lat());
            Data::get_instance().write(Data::NAV_GNSS_POS_LAT, &temp);
            temp = gps.location.lng();
            Data::get_instance().write(Data::NAV_GNSS_POS_LNG, &temp);
        }
        if (gps.altitude.isValid()) {
            double temp(gps.altitude.meters());
            Data::get_instance().write(Data::NAV_GNSS_POS_ALT, &temp);
        }
        if (gps.course.isValid()) {
            double temp(gps.course.deg());
            Data::get_instance().write(Data::NAV_GNSS_COURSE, &temp);
        }
    }

    // TODO: Kalmann filter

    return true;
}

void Sensors::update_status() {
    uint8_t temp(adxl1.get_status());
    Data::get_instance().write(Data::NAV_SENSOR_ADXL1_STAT, &temp);
    temp = adxl2.get_status();
    Data::get_instance().write(Data::NAV_SENSOR_ADXL2_STAT, &temp);
    temp = bmi1.get_accel_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_ACCEL_STAT, &temp);
    temp = bmi2.get_accel_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_ACCEL_STAT, &temp);
    temp = bmi1.get_gyro_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_GYRO_STAT, &temp);
    temp = bmi2.get_gyro_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_GYRO_STAT, &temp);
    auto temp_bmp(bmp1.get_status());
    Data::get_instance().write(Data::NAV_SENSOR_BMP1_STAT, &temp_bmp);
    temp_bmp = bmp2.get_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMP2_STAT, &temp_bmp);
}