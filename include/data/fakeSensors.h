//
// Created by marin on 13/04/2024.
//

#ifndef FAKESENSORS_H
#define FAKESENSORS_H
#include <string>


struct Vector3 {
    double x;
    double y;
    double z;
};

struct SensFiltered {
    Vector3   speed{};
    Vector3   accel{};
    Vector3   attitude{};
    double baro;
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
    double    altitude;

    SensFiltered();
};

class FakeSensors {
public:
    FakeSensors();
    ~FakeSensors();

    void calibrate();
    bool update();
    void set_data(std::string data);

    inline SensFiltered dump() const { return clean_data; }
private:
    SensFiltered clean_data;


    // Read sensors status
    void update_status();
};

#endif //FAKESENSORS_H
