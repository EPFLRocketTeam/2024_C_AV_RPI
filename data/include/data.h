#ifndef DATA_H
#define DATA_H


#include <cstdint>
#include "bmi08x.h"
#include "bmp3.h"
#include "I2CGPS.h"
#include "TinyGPS++.h"
#include "adxl375.h"
#include "PacketDefinition.h"


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

class Telecom;
struct UplinkCmd {
    CMD_ID id;
    uint8_t value;
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
struct DataDump {
    UplinkCmd telecom_status;
    SensStatus sensors_status;
    SensRaw sensors_raw;
    SensFiltered sensors_data;
};

class Data {
public:
    Data();
    ~Data();

    void send(Telecom m_telecom);

    DataDump dump() const;
    bool write_raw(SensRaw raw);
    bool write_filtered(SensFiltered filtered);
    DataDump goat;
private:
    bool updated;


};


#endif /* DATA_H */