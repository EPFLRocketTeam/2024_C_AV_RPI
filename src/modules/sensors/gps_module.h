#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include "module.h"
#include "I2CGPS.h"
#include "TinyGPS++.h"
#include "dynconf.h"

struct GPSModule : public SensorModule {
public:
    GPSModule (const char* module_name, const char* module_config);

    static GPSModule* make_gps();
private:
    I2CGPS*      i2c_gps  = nullptr;
    TinyGPSPlus* tiny_gps = nullptr;
    
    bool run_init() override;
    bool run_update() override;
    bool run_calibration() override;

    int low_polling_time() override { return 900; };
    int high_polling_time() override { return 900; };
};

#endif /* GPS_MODULE_H */