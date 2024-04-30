//
// Created by marin on 13/04/2024.
//

#ifndef FAKEDATA_H
#define FAKEDATA_H


#include "fakeTelecom.h"
#include "fakeSensors.h"

struct DataDump
{
    UPLink telecom_status{};
    SensFiltered sensors_data;
    bool calibrated;
};

class FakeData
{
public:
    FakeData();
    ~FakeData();

    bool update(const std::string& data);

    DataDump dump() const;

private:
    bool calibrated() const;
    bool updated;
    FakeTelecom m_telecom;
    FakeSensors m_sensors;
};


#endif //FAKEDATA_H
