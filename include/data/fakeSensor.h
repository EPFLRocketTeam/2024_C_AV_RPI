#ifndef FAKESENSOR_H
#define FAKESENSOR_H





struct Vector3 {
    double x;
    double y;
    double z;
};

struct SensFiltered {
    Vector3   speed;
    Vector3   accel;
    Vector3   attitude;
    double baro_atm[2];
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


class FakeSensors {    
public:
    FakeSensors();
    ~FakeSensors();


    void set_clean_data(SensFiltered data);

    void calibrate();
    bool update();
  
    SensFiltered dump() const;
private:

    SensFiltered clean_data;
 
};

#endif