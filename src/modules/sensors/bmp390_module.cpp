#include "bmp390_module.h"
#include "bmp3.h"

bool Bmp390Module::run_init() {
    bmp = new Bmp390(i2c_address);
    return bmp != nullptr;
}

bool Bmp390Module::run_update() {
    if (!bmp) {
        return false;
    }

    auto status = bmp->get_status();
    auto data = bmp->get_sensor_data();

    Data::get_instance().write(stat_reg, &status);
    Data::get_instance().write(data_reg, &data);

    return true;
}

bool Bmp390Module::run_calibration() {
    return true;
}

Bmp390Module::Bmp390Module(
    const char* module_name, 
    const char* module_config, 
    uint32_t i2c_address,
    Data::GoatReg stat_reg,
    Data::GoatReg data_reg
)
:   SensorModule(module_name, module_config),
    i2c_address(i2c_address),
    stat_reg(stat_reg),
    data_reg(data_reg)
{}

Bmp390Module* Bmp390Module::make_primary() {
    return new Bmp390Module(
        "Sensors::Bmp390.primary",
        "sensors.bmp1",
        BMP3_ADDR_I2C_PRIM,
        Data::GoatReg::NAV_SENSOR_BMP1_STAT,
        Data::GoatReg::NAV_SENSOR_BMP1_DATA
    );
}

Bmp390Module* Bmp390Module::make_secondary() {
    return new Bmp390Module(
        "Sensors::Bmp390.secondary",
        "sensors.bmp2",
        BMP3_ADDR_I2C_SEC,
        Data::GoatReg::NAV_SENSOR_BMP2_STAT,
        Data::GoatReg::NAV_SENSOR_BMP2_DATA
    );
}
