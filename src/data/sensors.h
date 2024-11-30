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

class Sensors  {
public:
    Sensors(Adxl375_Interface& adxl1_ref, Adxl375_Interface& adxl2_ref,
        Bmi088_Interface& bmi1_ref, Bmi088_Interface& bmi2_ref,
        Bmp3_Interface& bmp1_ref, Bmp3_Interface& bmp2_ref,
        I2CGPS_Interface& i2cgps_ref, TinyGPSPlus_Interface& gps_ref); 
    ~Sensors() ;
    void calibrate() ;
    bool update() ;
    // inline SensStatus get_status() const { return status; }
    // inline SensRaw get_raw() const { return raw_data; }
    // inline SensFiltered get_clean() const { return clean_data; }
    // Public accessors for testing
    Adxl375_Interface& get_adxl1() { return adxl1; }
    Adxl375_Interface& get_adxl2() { return adxl2; }
    Bmi088_Interface& get_bmi1() { return bmi1; }
    Bmi088_Interface& get_bmi2() { return bmi2; }
    Bmp3_Interface& get_bmp1() { return bmp1; }
    Bmp3_Interface& get_bmp2() { return bmp2; }
    I2CGPS_Interface& get_i2cgps() { return i2cgps; }
    TinyGPSPlus_Interface& get_gps() { return gps; }
    void update_status();

private:
    Adxl375_Interface& adxl1;
    Adxl375_Interface& adxl2;
    Bmi088_Interface& bmi1;
    Bmi088_Interface& bmi2;
    Bmp3_Interface& bmp1;
    Bmp3_Interface& bmp2;
    I2CGPS_Interface& i2cgps;
    TinyGPSPlus_Interface& gps;

    // SensStatus status;
    // SensRaw raw_data;
    // SensFiltered clean_data;

    // Read sensors status
};

#endif /* SENSORS_H */