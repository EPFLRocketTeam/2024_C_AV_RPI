#include "bmi088_module.h"
#include "bmi08x.h"

bool Bmi088Module::run_init() {
    bmi = new Bmi088(accel_i2c_address, gyro_i2c_address);
    return bmi != nullptr;
}

bool Bmi088Module::run_update() {
    if (!bmi) {
        return false;
    }

    uint8_t gyro_status  = bmi->get_gyro_status();
    uint8_t accel_status = bmi->get_accel_status();

    auto gyro_data  = bmi->get_gyro_data();
    auto accel_data = bmi->get_accel_data();

    Data::get_instance().write(accel_stat_reg, &accel_status);
    Data::get_instance().write(gyro_stat_reg,  &gyro_status);
    Data::get_instance().write(accel_data_reg, &accel_data);
    Data::get_instance().write(gyro_data_reg,  &gyro_data);

    return true;
};

bool Bmi088Module::run_calibration() {
    return true;
};

Bmi088Module::Bmi088Module(
    const char* module_name, 
    const char* module_config, 
    uint32_t gyro_i2c_address, 
    uint32_t accel_i2c_address,
    Data::GoatReg gyro_stat_reg,
    Data::GoatReg gyro_data_reg,
    Data::GoatReg accel_stat_reg,
    Data::GoatReg accel_data_reg
)
:   SensorModule(module_name, module_config),
    gyro_i2c_address(gyro_i2c_address),
    accel_i2c_address(accel_i2c_address),
    gyro_stat_reg(gyro_stat_reg),
    gyro_data_reg(gyro_data_reg),
    accel_stat_reg(accel_stat_reg),
    accel_data_reg(accel_data_reg)
{}

Bmi088Module* Bmi088Module::make_primary() {
    return new Bmi088Module(
        "Sensors::Bmi088.primary",
        "sensors.bmi1",
        BMI08_GYRO_I2C_ADDR_PRIMARY,
        BMI08_ACCEL_I2C_ADDR_PRIMARY,
        Data::GoatReg::NAV_SENSOR_BMI1_GYRO_STAT,
        Data::GoatReg::NAV_SENSOR_BMI1_GYRO_DATA,
        Data::GoatReg::NAV_SENSOR_BMI1_ACCEL_STAT,
        Data::GoatReg::NAV_SENSOR_BMI1_ACCEL_DATA
    );
}

Bmi088Module* Bmi088Module::make_secondary() {
    return new Bmi088Module(
        "Sensors::Bmi088.secondary",
        "sensors.bmi2",
        BMI08_GYRO_I2C_ADDR_SECONDARY,
        BMI08_ACCEL_I2C_ADDR_SECONDARY,
        Data::GoatReg::NAV_SENSOR_BMI2_GYRO_STAT,
        Data::GoatReg::NAV_SENSOR_BMI2_GYRO_DATA,
        Data::GoatReg::NAV_SENSOR_BMI2_ACCEL_STAT,
        Data::GoatReg::NAV_SENSOR_BMI2_ACCEL_DATA
    );
}
