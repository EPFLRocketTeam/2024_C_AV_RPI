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

bool FakeData::update()
{
    m_telecom.update();
    updated = m_sensors.update();

    return updated;
}

DataDump FakeData::dump() const
{
    return {m_telecom.get_cmd(), m_sensors.dump()};
}

// Path: src/data/fakeTelecom.cpp

