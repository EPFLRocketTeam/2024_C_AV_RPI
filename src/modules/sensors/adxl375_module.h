
#include "module.h"
#include "adxl375.h"
#include "dynconf.h"

struct Adxl375Module : public SensorModule {
private:
    Adxl375* adxl = NULL;
    
    uint32_t i2c_address;

    Data::GoatReg stat_reg;
    Data::GoatReg data_reg;

    bool run_init () override;
    bool run_update () override;
    bool run_calibration () override;
public:
    Adxl375Module (
        const char* module_name, 
        const char* module_config, 
        uint32_t i2c_address,
        Data::GoatReg stat_reg,
        Data::GoatReg data_reg
    );

    static Adxl375Module* make_primary ();
    static Adxl375Module* make_secondary ();
};
