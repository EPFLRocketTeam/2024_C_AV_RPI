//
// Created by marin on 13/04/2024.
//

#ifndef FAKEDATA_H
#define FAKEDATA_H



#include "fakeTelecom.h"
#include "fakeSensors.h"

struct DataDump {
    UPLink telecom_status;
    SensFiltered sensors_data;
};

class FakeData {
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



#endif //FAKEDATA_H
