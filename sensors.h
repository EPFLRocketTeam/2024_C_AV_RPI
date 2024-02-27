#ifndef SENSORS_H
#define SENSORS_H

struct SensorsData {
    float gnss_lon;
    float gnss_lat;
    float gnss_alt;
    float gnss_lon_r;
    float gnss_lat_r;
    float gnss_atl_r;
    float gnss_vertical_speed;
    float N2_pressure;
    float fuel_pressure;
    float LOX_pressure;
    float fuel_level;
    float LOX_level;
    float engine_temp;
    float igniter_pressure;
    float LOX_inj_pressure;
    float fuel_inj_pressure;
    float chamber_pressure;
};

class Sensors {
public:
    Sensors() {}
    ~Sensors() {}

    bool update();

    inline SensorsData dump() const { return m_data; }
private:
    SensorsData m_data;
};

#endif /* SENSORS_H */