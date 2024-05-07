#ifndef DATA_H
#define DATA_H

// J'ai enleve tout ce qui est telecom

//#include "telecom.h"
#include "sensors.h"

struct DataDump {
    //UPLink telecom_status;
    SensFiltered sensors_data;
};

class Data {
public:
    Data();
    ~Data();

    bool update();
    DataDump dump() const;

    // --- Added by me ---
    float get_time();
    Sensors get_sensors();
    
    void set_time(float);

    void set_adxl_status(uint8_t);
    void set_adxl_aux_status(uint8_t);
    void set_bmi_accel_status(uint8_t);
    void set_bmi_aux_accel_status(uint8_t);
    void set_bmi_gyro_status(uint8_t);
    void set_bmi_aux_gyro_status(uint8_t);
    void set_bmp_status(bmp3_status);
    void set_bmp_aux_status(bmp3_status);

    void set_adxl(adxl375_data);
    void set_adxl_aux(adxl375_data);
    void set_bmi_accel(bmi08_sensor_data_f);
    void set_bmi_aux_accel(bmi08_sensor_data_f);
    void set_bmi_gyro(bmi08_sensor_data_f);
    void set_bmi_aux_gyro(bmi08_sensor_data_f);
    void set_bmp(bmp3_data);
    void set_bmp_aux(bmp3_data);

    void set_altitude(double);
    void set_velocity(Vector3);
private:
    bool updated;
    //Telecom m_telecom;
    Sensors m_sensors;

    // added by me
    float time; // time in seconds
};


#endif /* DATA_H */