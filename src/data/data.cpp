#include "data.h"

Data::Data() {}

Data::~Data() {}

bool Data::update() {
    m_telecom.update();
    updated = m_sensors.update();

    return updated;
}

DataDump Data::dump() const {
    return {m_telecom.get_cmd(), m_sensors.dump()};
}