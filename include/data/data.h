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

    bool update();
    DataDump dump() const;

private:
    bool calibrated() const;
    bool updated;
    std::unique_ptr<Telecom> m_telecom;
    std::unique_ptr<Sensors> m_sensors;
};

#endif /* DATA_H */