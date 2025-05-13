
#include "module.h"
#include "adxl375.h"
#include "dynconf.h"

enum Adxl375SensorKind : uint8_t {
    PRIMARY   = 0,
    SECONDARY = 1
};

const char* adxl_modules_names[2] = {
    "Sensors::Adxl.Primary",
    "Sensors::Adxl.Seconday"
};
const char* adxl_modules_config[2] = {
    "sensors.adxl1",
    "sensors.adxl2"
};
const uint32_t adxl_modules_i2c_address[2] = {
    ADXL375_ADDR_I2C_PRIM,
    ADXL375_ADDR_I2C_SEC
};

struct Adxl375Module : public SensorModule {
private:
    Adxl375* adxl = NULL;
    Adxl375SensorKind sensor_kind;

    bool run_init () override;
    bool run_update () override;
    bool run_calibration () override;
public:
    Adxl375Module (Adxl375SensorKind kind);
};
