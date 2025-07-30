#ifndef BMP390_MODULE_H
#define BMP390_MODULE_H

#include "module.h"
#include "dynconf.h"
#include "data.h"

class Bmp390;

struct Bmp390Module : public SensorModule {
public:
    Bmp390Module(
        const char* module_name, 
        const char* module_config, 
        uint32_t i2c_address, 
        Data::GoatReg stat_reg,
        Data::GoatReg data_reg
    );
    
    static Bmp390Module* make_primary();
    static Bmp390Module* make_secondary();
private:
    Bmp390* bmp = nullptr;

    Data::GoatReg stat_reg;
    Data::GoatReg data_reg;
    
    uint32_t i2c_address;
    
    bool run_init() override;
    bool run_update() override;
    bool run_calibration() override;
};

#endif /* BMP390_MODULE_H */