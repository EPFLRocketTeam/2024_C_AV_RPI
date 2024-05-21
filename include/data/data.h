#ifndef DATA_H
#define DATA_H

#include "telecom.h"
#include "sensors.h"

struct FlightData {
    UplinkCmd telecom_status;
    SensRaw sensors_raw;
    SensFiltered sensors_data;
};

class Data {
public:
    Data();
    ~Data();

    void send();
    bool update();
    FlightData get() const;
private:
    bool updated;
    Telecom m_telecom;
    Sensors m_sensors;
};


#endif /* DATA_H */