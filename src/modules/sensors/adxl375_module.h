#ifndef ADXL375_MODULE_H
#define ADXL375_MODULE_H

#include "module.h"
#include "dynconf.h"
#include "data.h"

class Adxl375;

struct Adxl375Module : public SensorModule {
public:
    Adxl375Module(
        const char* module_name, 
        const char* module_config, 
        uint32_t i2c_address,
        Data::GoatReg stat_reg,
        Data::GoatReg data_reg
    );

    static Adxl375Module* make_primary();
    static Adxl375Module* make_secondary();
private:
    Adxl375* adxl = nullptr;
    
    uint32_t i2c_address;

    Data::GoatReg stat_reg;
    Data::GoatReg data_reg;

    bool run_init() override;
    bool run_update() override;
    bool run_calibration() override;
};

#endif /* ADXL375_MODULE_H */