#ifndef BMI088_MODULE_H
#define BMI088_MODULE_H

#include "module.h"
#include "dynconf.h"
#include "data.h"

class Bmi088;

struct Bmi088Module : public SensorModule {
public:
    Bmi088Module(
        const char* module_name, 
        const char* module_config, 
        uint32_t gyro_i2c_address, 
        uint32_t accel_i2c_address,
        Data::GoatReg gyro_stat_reg,
        Data::GoatReg gyro_data_reg,
        Data::GoatReg accel_stat_reg,
        Data::GoatReg accel_data_reg
    );
    
    static Bmi088Module* make_primary();
    static Bmi088Module* make_secondary();
private:
    Bmi088* bmi = nullptr;

    uint32_t gyro_i2c_address;
    uint32_t accel_i2c_address;

    Data::GoatReg gyro_stat_reg;
    Data::GoatReg gyro_data_reg;
    Data::GoatReg accel_stat_reg;
    Data::GoatReg accel_data_reg;

    bool run_init() override;
    bool run_update() override;
    bool run_calibration() override;
};

#endif /* BMI088_MODULE_H */
