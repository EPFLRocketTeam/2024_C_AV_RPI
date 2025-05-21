
#include "module.h"
#include "INA228.h"
#include "dynconf.h"

struct INA228Module : public SensorModule {
private:
    INA228* ina = NULL;
    
    uint32_t i2c_address;

    Data::GoatReg voltage_reg;

    float shunt;
    float maxCurrent;

    bool run_init () override;
    bool run_update () override;
    bool run_calibration () override;
public:
    INA228Module (
        const char* module_name, 
        const char* module_config, 
        uint32_t i2c_address,
        Data::GoatReg voltage_reg,
        float shunt,
        float maxCurrent
    );
    
    static INA228Module* make_lpb ();
    static INA228Module* make_hpb ();
};
