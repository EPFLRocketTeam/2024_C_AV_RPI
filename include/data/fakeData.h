
#include "data.h"
#include "fakeTelecom.h"
#include "fakeSensors.h"
#include <iostream>
#include <string>

class FakeData : public Data
{

public:
    FakeData();
    ~FakeData();
    void set_data(const std::string data);
    bool update() override;

private:
    DataDump m_data;
    std::string m_raw;
    FakeTelecom m_telecom;
    FakeSensors m_sensors;

};