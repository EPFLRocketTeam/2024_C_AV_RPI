#ifndef SENSORS_H
#define SENSORS_H

#include <cstdint>
#include "bmi08x.h"
#include "bmp3.h"
#include "I2CGPS.h"
#include "TinyGPS++.h"
#include "adxl375.h"
#include "data.h"




class Sensors {
public:
    Sensors(Data data);
    ~Sensors();

    void calibrate();
    bool update();
    bool write();
    inline SensStatus get_status() const { return status; }
    inline SensRaw get_raw() const { return raw_data; }
    inline SensFiltered get_clean() const { return clean_data; }
private:
    Data data;
    Adxl375 adxl1, adxl2;
    Bmi088 bmi1, bmi2;
    Bmp390 bmp1, bmp2;
    I2CGPS i2cgps;
    TinyGPSPlus gps;
    SensStatus status;
    SensRaw raw_data;
    SensFiltered clean_data;

    // Read sensors status
    void update_status();
};

#endif /* SENSORS_H */