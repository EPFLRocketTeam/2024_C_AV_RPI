//
// Created by marin on 13/04/2024.
//

#include "../include/data/fakeData.h"


FakeData::FakeData() : updated(false)
{
}

FakeData::~FakeData()
{
}

bool FakeData::update(const std::string& data)
{
    m_telecom.update(data);
    updated = m_sensors.update(data);

    return updated;
}


DataDump FakeData::dump() const
{
    return {m_telecom.get_cmd(), m_sensors.dump(), calibrated()};
}

bool FakeData::calibrated()
{
    SensStatus s = m_sensors.get_status();
    return s.adxl_status && s.adxl_aux_status && s.bmi_status && s.bmi_aux_status;
}

// Path: src/data/fakeTelecom.cpp

