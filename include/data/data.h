#ifndef DATA_H
#define DATA_H

#include "telecom.h"
#include "sensors.h"

struct DataDump {
    UPLink telecom_status;
    SensFiltered sensors_data;
};

class Data {
public:
    Data();
    ~Data();

    bool update();
    DataDump dump() const;
private:
    bool updated;
    Telecom m_telecom;
    Sensors m_sensors;
};


#endif /* DATA_H */