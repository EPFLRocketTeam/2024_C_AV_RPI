#include <iostream>
#include "sensors.h"
#include "av_timer.h"
#include "INA228.h"
#include "TMP1075.h"
#include "kalman_params.h"
#include "config.h"
#include "data.h"
#include "logger.h"
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

    AvTimer::sleep(100);
    sensors.push_back( Bmi088Module::make_primary() );
    AvTimer::sleep(100);
    sensors.push_back( Bmi088Module::make_secondary() );
    AvTimer::sleep(100);

    sensors.push_back( Bmp390Module::make_primary() );
    sensors.push_back( Bmp390Module::make_secondary() );

    sensors.push_back( INA228Module::make_lpb() );
    sensors.push_back( INA228Module::make_hpb_trb() );
    sensors.push_back( INA228Module::make_hpb_prb() );

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

    return result;
}

void Sensors::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    for (SensorModule* mod : sensors)
        mod->check_policy(dump, delta_ms);
    
    if (dump.av_state == State::CALIBRATION) {
        bool calibrated(true);

        Data::get_instance().write(Data::EVENT_CALIBRATED, &calibrated);
    }

    float amb_temp(dump.sens.bmp_aux.temperature);
    Data::get_instance().write(Data::AV_AMB_TEMPERATURE, &amb_temp);

    float acc_x((dump.sens.bmi_accel.x + dump.sens.bmi_aux_accel.x) * 0.5);
    float acc_y((dump.sens.bmi_accel.y + dump.sens.bmi_aux_accel.y) * 0.5);
    float acc_z((dump.sens.bmi_accel.z + dump.sens.bmi_aux_accel.z) * 0.5);
    Vector3 acceleration{acc_x, acc_y, acc_z};
    Data::get_instance().write(Data::NAV_ACCELERATION, &acceleration);
    Logger::log_eventf(Logger::DEBUG, "Acceleration Avg: X: %f | Y: %f | Z: %f", acceleration.x, acceleration.y, acceleration.z);

    // kalman checks if we are static for calibration
    // kalman.check_static(dump);
    return;
}
