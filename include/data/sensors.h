#ifndef SENSORS_H
#define SENSORS_H

#include "adxl375.h"
#include "bmi08x.h"
#include "bmp3.h"

struct SensStatus {
    uint8_t     adxl_status;
    uint8_t     adxl_aux_status;
    uint8_t     bmi_status;
    uint8_t     bmi_aux_status;
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

struct SensFiltered {
    Vector3   speed;
    Vector3   accel;
    Vector3   attitude;
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
    bool ignited;

    SensFiltered();
};

class Sensors {
public:
    Sensors();
    ~Sensors();

    void calibrate();
    bool update();
    inline SensStatus get_status() const { return status; }
    inline SensFiltered dump() const { return clean_data; }
private:
    Adxl375 adxl1, adxl2;
    Bmi088 bmi1, bmi2;
    Bmp390 bmp1, bmp2;
    SensStatus status;
    SensRaw raw_data;
    SensFiltered clean_data;

    // Read sensors status
    void update_status();
};

#endif /* SENSORS_H */