#ifndef SENSORS_H
#define SENSORS_H

#include "adxl375.h"
#include "bmi08x.h"
#include "bmp3.h"
#include "I2CGPS.h"
#include "TinyGPS++.h"

struct SensStatus {
    uint8_t     adxl_status;
    uint8_t     adxl_aux_status;
    uint8_t     bmi_accel_status;
    uint8_t     bmi_aux_accel_status;
    uint8_t     bmi_gyro_status;
    uint8_t     bmi_aux_gyro_status;
    bmp3_status bmp_status;
    bmp3_status bmp_aux_status;
};

struct SensRaw {
    adxl375_data        adxl;
    adxl375_data        adxl_aux;
    bmi08_sensor_data_f bmi_accel;
    bmi08_sensor_data_f bmi_gyro; // Correspondance x,y,z / roll,pitch,yaw à déterminer
    bmi08_sensor_data_f bmi_aux_accel;
    bmi08_sensor_data_f bmi_aux_gyro;
    bmp3_data           bmp;
    bmp3_data           bmp_aux;

    SensRaw();
};

struct Vector3 {
    double x;
    double y;
    double z;
};

struct GPSCoord {
    double lat;
    double lng;
    double alt;
};

struct GPSTime {
    unsigned year;
    unsigned month;
    unsigned day;
    unsigned hour;
    unsigned minute;
    unsigned second;
    unsigned centisecond;
};

struct SensFiltered {
    GPSTime   time;
    GPSCoord  position;
    Vector3   speed;
    Vector3   accel;
    Vector3   attitude;
    double    course;
    double    altitude;
    bmp3_data baro;
    double    N2_pressure;
    double    fuel_pressure;
    double    LOX_pressure;
    double    fuel_level;
    double    LOX_level;
    double    engine_temperature;
    double    igniter_pressure;
    double    LOX_inj_pressure;
    double    fuel_inj_pressure;
    double    chamber_pressure;

    SensFiltered();
};

class Sensors {
public:
    Sensors();
    ~Sensors();

    void calibrate();
    bool update();
    inline SensStatus get_status() const { return status; }
    inline SensRaw get_raw() const { return raw_data; }
    inline SensFiltered get_clean() const { return clean_data; }
private:
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