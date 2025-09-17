#include "config.h"
#include "ina228_module.h"
#include "INA228.h"

bool INA228Module::run_init() {
    ina = new INA228(i2c_address, shunt, maxCurrent);
    return ina != nullptr;
}

bool INA228Module::run_update() {
    if (!ina) {
        return false;
    }

    float voltage(ina->getBusVoltage());
    float current(ina->getCurrent());

    Data::get_instance().write(voltage_reg, &voltage);
    Data::get_instance().write(current_reg, &current);

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
    Data::GoatReg current_reg,
    float shunt,
    float maxCurrent
)
:   SensorModule(module_name, module_config),
    i2c_address(i2c_address),
    voltage_reg(voltage_reg),
    current_reg(current_reg),
    shunt(shunt),
    maxCurrent(maxCurrent)
{}

INA228Module* INA228Module::make_lpb() {
    return new INA228Module(
        "Sensors::Ina228.LPB",
        "sensors.ina228_lpb",
        INA228_ADDRESS_LPB,
        Data::GoatReg::BAT_LPB_VOLTAGE,
        Data::GoatReg::BAT_LPB_CURRENT,
        INA228_LPB_SHUNT,
        INA228_LPB_MAX_CUR
    );
}

INA228Module* INA228Module::make_hpb_trb() {
    return new INA228Module(
        "Sensors::Ina228.HPB_TRB",
        "sensors.ina228_hpb_trb",
        INA228_ADDRESS_HPB_TRB,
        Data::GoatReg::BAT_HPB_VOLTAGE_TRB,
        Data::GoatReg::BAT_HPB_CURRENT_TRB,
        INA228_HPB_TRB_SHUNT,
        INA228_HPB_TRB_MAX_CUR
    );
}

INA228Module* INA228Module::make_hpb_prb() {
    return new INA228Module(
        "Sensors::Ina228.HPB_PRB",
        "sensors.ina228_hpb_prb",
        INA228_ADDRESS_HPB_PRB,
        Data::GoatReg::BAT_HPB_VOLTAGE_PRB,
        Data::GoatReg::BAT_HPB_CURRENT_PRB,
        INA228_HPB_PRB_SHUNT,
        INA228_HPB_PRB_MAX_CUR
    );
}
