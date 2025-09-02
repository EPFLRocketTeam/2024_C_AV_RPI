#include "tmp1075_module.h"
#include "TMP1075.h"
#include "data.h"
#include "logger.h"

bool Tmp1075Module::run_init() {
    tmp1075 = new TMP1075(TMP1075_ADDR_I2C);
    return tmp1075 != nullptr;
}

bool Tmp1075Module::run_update() {
    if (!tmp1075) {
        return false;
    }

    float fc_temperature (tmp1075->getTemperatureCelsius());
    Logger::log_event(Logger::DEBUG, "TEST");

    Data::get_instance().write(Data::AV_FC_TEMPERATURE, &fc_temperature);

    return true;
}

bool Tmp1075Module::run_calibration() {
    return true;
}

Tmp1075Module::Tmp1075Module(const char* module_name, const char* module_config)
:    SensorModule(module_name, module_config) {}

Tmp1075Module* Tmp1075Module::make_tmp() {
    return new Tmp1075Module(
        "Sensors::Tmp1075",
        "sensors.tmp1075"
    );
}
