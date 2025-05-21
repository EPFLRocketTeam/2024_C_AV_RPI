
#include "module.h"
#include "I2CGPS.h"
#include "TinyGPS++.h"
#include "dynconf.h"

struct GPSModule : public SensorModule {
private:
    I2CGPS*      i2c_gps  = NULL;
    TinyGPSPlus* tiny_gps = NULL;
    
    bool run_init () override;
    bool run_update () override;
    bool run_calibration () override;
public:
    GPSModule (
        const char* module_name, 
        const char* module_config
    );

    static GPSModule* make_gps ();
};