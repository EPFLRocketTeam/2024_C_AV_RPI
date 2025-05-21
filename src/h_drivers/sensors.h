// Paradigm:
// Either get all sensors status/values, then write them to the GOAT
// Or implement in each sensor driver to do it directly on its own.
// Either way, do we still need to store sensors status, raw navigation values,
// and filtered global sensors values as members of this class ? Since we have the GOAT,
// can't we directly write them to it upon sensors acquisition, i.e. in Sensors::update() ?
// This typically would look like: 
//  Data::get_instance().write(ADXL1_RAW_DATA, adxl1.get_data());
// That way, Sensors is just a wrapper to manage the sensor drivers, without storing anything.

#ifndef SENSORS_H
#define SENSORS_H

#include <vector>
#include "bmi08x.h"
#include "bmp3.h"
#include "I2CGPS.h"
#include "TinyGPS++.h"
#include "adxl375.h"
#include "h_driver.h"
#include "kalman.h"
#include "INA228.h"
#include "TMP1075.h"
#include "module.h"

class Sensors : public HDriver {
public:
    Sensors();
    ~Sensors();

    void check_policy(const DataDump& dump, const uint32_t delta_ms) override;

    void init_sensors ();

    // inline SensStatus get_status() const { return status; }
    // inline SensRaw get_raw() const { return raw_data; }
    // inline SensFiltered get_clean() const { return clean_data; }
private:
    std::vector<SensorModule*> sensors;

    // SensStatus status;
    // SensRaw raw_data;
    // SensFiltered clean_data;

    // Kalman kalman;
};

#endif /* SENSORS_H */
