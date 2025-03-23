#include "sensors.h"
#include "INA228.h"
#include "data.h"

Sensors::Sensors()
:   adxl1(ADXL375_ADDR_I2C_PRIM),
    adxl2(ADXL375_ADDR_I2C_SEC),
    bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY),
    bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY),
    bmp1(BMP3_ADDR_I2C_PRIM),
    bmp2(BMP3_ADDR_I2C_SEC),
    i2cgps(),
    gps(),
    ina_lpb(INA228_ADDRESS_LPB),
    ina_hpb(INA228_ADDRESS_HPB)
{
    update_status();
}

Sensors::~Sensors() {}

void Sensors::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    // Everytime a new command is received we write to the goat

    // TODO: Implement the logic for the sensors driver
    return;
}

//TODO: must return bool to written into goat.event
void Sensors::calibrate() {
    //must have counter ro return an error if too much
    //Redo calibration
    adxl1.calibrate();
    adxl2.calibrate();
}

bool Sensors::update() {
    update_status();

    try {
        float lpb_voltage(ina_lpb.getBusVoltage());
        Data::get_instance().write(Data::BAT_LPB_VOLTAGE, &lpb_voltage);
    }catch(INA228Exception& e) {
        std::cout << "INA228 LPB: " << e.what() << "\n";
    }

    try {
        float hpb_voltage(ina_hpb.getBusVoltage());
        Data::get_instance().write(Data::BAT_HPB_VOLTAGE, &hpb_voltage);
    }catch(INA228Exception& e) {
        std::cout << "INA228 HPB: " << e.what() << "\n";
    }

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
    if (gps.time.isUpdated()) {
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
