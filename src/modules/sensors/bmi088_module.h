
#include "module.h"
#include "bmi08x.h"
#include "dynconf.h"

struct Bmi088Module : public SensorModule {
private:
    Bmi088* bmi = NULL;

    Data::GoatReg gyro_stat_reg;
    Data::GoatReg gyro_data_reg;
    Data::GoatReg accel_stat_reg;
    Data::GoatReg accel_data_reg;
    
    uint32_t gyro_i2c_address;
    uint32_t accel_i2c_address;

    bool run_init () override;
    bool run_update () override;
    bool run_calibration () override;
public:
    Bmi088Module (
        const char* module_name, 
        const char* module_config, 
        uint32_t gyro_i2c_address, 
        uint32_t accel_i2c_address,
        Data::GoatReg gyro_stat_reg,
        Data::GoatReg gyro_data_reg,
        Data::GoatReg accel_stat_reg,
        Data::GoatReg accel_data_reg
    );
    
    static Bmi088Module* make_primary ();
    static Bmi088Module* make_secondary ();
};
