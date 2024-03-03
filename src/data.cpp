#include "data.h"

bool Data::update() {
    m_telecom.update();
    updated = m_sensors.update();

    return updated;
}

DataDump Data::dump() const {
    DataDump out;
    out.telecom_status = m_telecom.get_cmd();
    out.sensors_data = m_sensors.dump();
    return out;
}