#include <iostream>
#include "sensors.h"
#include "INA228.h"
#include "TMP1075.h"
#include "kalman_params.h"
#include "config.h"
#include "data.h"

#include "adxl375_module.h"
#include "bmi088_module.h"
#include "bmp390_module.h"
#include "ina228_module.h"
#include "gps_module.h"
#include "tmp1075_module.h"

Sensors::Sensors()
/* :   kalman(INITIAL_COV_GYR_BIAS,
           INITIAL_COV_ACCEL_BIAS,
           INITIAL_COV_ORIENTATION,
           GYRO_COV,
           GYRO_BIAS_COV,
           ACCEL_COV,
           ACCEL_BIAS_COV,
           GPS_OBS_COV,
           ALT_OBS_COV) */ {}

void Sensors::init_sensors () {
    sensors.push_back( Adxl375Module::make_primary() );
    sensors.push_back( Adxl375Module::make_secondary() );

    sensors.push_back( Bmi088Module::make_primary() );
    sensors.push_back( Bmi088Module::make_secondary() );

    sensors.push_back( Bmp390Module::make_primary() );
    sensors.push_back( Bmp390Module::make_secondary() );

    sensors.push_back( INA228Module::make_lpb() );
    sensors.push_back( INA228Module::make_hpb() );

    sensors.push_back( GPSModule::make_gps() );

    sensors.push_back( Tmp1075Module::make_tmp() );

    for (SensorModule* module : sensors)
        module->init();
}

Sensors::~Sensors() {}

std::map<std::string, bool> Sensors::sensors_status () {
    std::map<std::string, bool> result;

    for (SensorModule* module : sensors) {
        result[module->get_name()] = module->is_enabled() && !module->is_failure();
    }
}

void Sensors::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    for (SensorModule* mod : sensors)
        mod->check_policy(dump, delta_ms);
    
    if (dump.av_state == State::CALIBRATION) {
        bool calibrated(true);

        Data::get_instance().write(Data::EVENT_CALIBRATED, &calibrated);
    }

    // kalman checks if we are static for calibration
    // kalman.check_static(dump);
    return;
}
