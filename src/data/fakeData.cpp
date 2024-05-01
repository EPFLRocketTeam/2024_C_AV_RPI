//
// Created by marin on 30/04/2024.
//

#include "../include/data/fakeData.h"

FakeData::FakeData() : Data() {}

FakeData::~FakeData() {}

void FakeData::set_data(const std::string data) {
   this->m_sensors.set_data(data);
   this->m_telecom.set_update(data);
}

bool FakeData::update() {
    m_telecom.update();
    bool updated = m_sensors.update();
    return updated;
}



