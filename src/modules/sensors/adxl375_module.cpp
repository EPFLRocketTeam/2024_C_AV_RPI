
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

    if (sensor_kind == Adxl375SensorKind::PRIMARY)
    {
        Data::get_instance().write(Data::NAV_SENSOR_ADXL1_STAT, &status);
        Data::get_instance().write(Data::NAV_SENSOR_ADXL1_DATA, &data);
    }
    else
    {
        Data::get_instance().write(Data::NAV_SENSOR_ADXL2_STAT, &status);
        Data::get_instance().write(Data::NAV_SENSOR_ADXL2_DATA, &data);
    }

    return true;
};
bool Adxl375Module::run_calibration()
{
    adxl->calibrate();

    return true;
};
Adxl375Module::Adxl375Module(Adxl375SensorKind kind)
    : SensorModule(
          adxl_modules_names[kind],
          adxl_modules_config[kind]),
      sensor_kind(kind) {}
