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

#include "bmi08x.h"
#include "bmp3.h"
#include "I2CGPS.h"
#include "TinyGPS++.h"
#include "adxl375.h"
#include "data.h"
#include "../DriverInterface.h"

class Sensors : public DriverInterface {
public:
    Sensors();
    ~Sensors();

    void calibrate();
    bool update();
    void checkPolicy(DataDump dump) override;
    // inline SensStatus get_status() const { return status; }
    // inline SensRaw get_raw() const { return raw_data; }
    // inline SensFiltered get_clean() const { return clean_data; }
private:
    Adxl375 adxl1, adxl2;
    Bmi088 bmi1, bmi2;
    Bmp390 bmp1, bmp2;
    I2CGPS i2cgps;
    TinyGPSPlus gps;

    // SensStatus status;
    // SensRaw raw_data;
    // SensFiltered clean_data;

    // Read sensors status
    void update_status();
};

struct timeCode {
    int second;
    int nanosecond;
};


#endif /* SENSORS_H */