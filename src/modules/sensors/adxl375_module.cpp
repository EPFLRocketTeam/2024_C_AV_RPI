
#include "adxl375_module.h"

bool Adxl375Module::run_init()
{
    adxl = new Adxl375(adxl_modules_i2c_address[sensor_kind]);
    if (adxl == NULL)
    {
        return false;
    }

    return true;
};
bool Adxl375Module::run_update()
{
    if (adxl == NULL)
    {
        return false;
    }

    uint8_t status = adxl->get_status();
    auto data = adxl->get_data();

    Data::get_instance().write(stat_reg, &status);
    Data::get_instance().write(data_reg, &data);

    return true;
};
bool Adxl375Module::run_calibration()
{
    if (adxl == NULL) {
        return false;
    }
    adxl->calibrate();

    return true;
};
Adxl375Module::Adxl375Module(
        const char* module_name, 
        const char* module_config, 
        uint32_t i2c_address,
        Data::GoatReg stat_reg,
        Data::GoatReg data_reg
    )
    : SensorModule(
          module_name,
          module_config),
      i2c_address(i2c_address),
      stat_reg(stat_reg),
      data_reg(data_reg) {}

Adxl375Module* Adxl375Module::make_primary () {
    return new Adxl375Module(
        "Sensors::Adxl375.primary",
        "sensors.adxl1",
        ADXL375_ADDR_I2C_PRIM,
        Data::GoatReg::NAV_SENSOR_ADXL1_STAT,
        Data::GoatReg::NAV_SENSOR_ADXL1_DATA
    );
}
Adxl375Module* Adxl375Module::make_secondary () {
    return new Adxl375Module(
        "Sensors::Adxl375.secondary",
        "sensors.adxl2",
        ADXL375_ADDR_I2C_SEC,
        Data::GoatReg::NAV_SENSOR_ADXL2_STAT,
        Data::GoatReg::NAV_SENSOR_ADXL2_DATA
    );
}
