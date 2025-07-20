#include "config.h"
#include "ina228_module.h"

bool INA228Module::run_init() {
    ina = new INA228(i2c_address, shunt, maxCurrent);
    return ina != nullptr;
}

bool INA228Module::run_update() {
    if (!ina) {
        return false;
    }

    float voltage (ina->getBusVoltage());

    Data::get_instance().write(voltage_reg, &voltage);

    return true;
}

bool INA228Module::run_calibration() {
    if (!ina) {
        return false;
    }

    ina->setMaxCurrentShunt(maxCurrent, shunt);
    
    return true;
}

INA228Module::INA228Module(
    const char* module_name, 
    const char* module_config, 
    uint32_t i2c_address,
    Data::GoatReg voltage_reg,
    float shunt,
    float maxCurrent
)
:   SensorModule(module_name, module_config),
    i2c_address(i2c_address),
    voltage_reg(voltage_reg),
    shunt(shunt),
    maxCurrent(maxCurrent)
{}

INA228Module* INA228Module::make_lpb() {
    return new INA228Module(
        "Sensors::Ina228.LPB",
        "sensors.ina228_lpb",
        INA228_ADDRESS_LPB,
        Data::GoatReg::BAT_LPB_VOLTAGE,
        INA228_LPB_SHUNT,
        INA228_LPB_MAX_CUR
    );
}

INA228Module* INA228Module::make_hpb() {
    return new INA228Module(
        "Sensors::Ina228.HPB",
        "sensors.ina228_hpb",
        INA228_ADDRESS_HPB,
        Data::GoatReg::BAT_HPB_VOLTAGE,
        INA228_HPB_SHUNT,
        INA228_HPB_MAX_CUR
    );
}