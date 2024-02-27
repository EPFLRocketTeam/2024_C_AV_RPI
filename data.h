#ifndef DATA_H
#define DATA_H

#include "telecom.h"
#include "battery.h"
#include "sensors.h"

struct DataDump {
    UPLink telecom_status;
    double bat_voltage;
    double bat_current;
    SensorsData sensors_data;
};

class Data {
public:
    Data();
    virtual ~Data();

    bool update();
    DataDump dump() const;
private:
    bool updated;
    Telecom m_telecom;
    Battery m_bat;
    Sensors m_sensors;
};


#endif /* DATA_H */