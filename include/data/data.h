#ifndef DATA_H
#define DATA_H

#include "telecom.h"
#include "sensors.h"

struct DataDump {
    UPLink telecom_status;
    SensFiltered sensors_data;
    bool calibrated;
};

class Data {
public:
    Data();
    ~Data();

    virtual bool update();
    DataDump dump() const;

private:
    bool calibrated() const;
    bool updated;
    Telecom m_telecom;
    Sensors m_sensors;
};


#endif /* DATA_H */