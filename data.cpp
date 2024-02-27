#include "data.h"

bool Data::update() {
    m_bat.update();
    m_telecom.update();
    updated = m_sensors.update();

    return updated;
}

DataDump Data::dump() const {
    DataDump out;
    out.telecom_status = m_telecom.get_cmd();
    out.bat_voltage = m_bat.get_voltage();
    out.bat_current = m_bat.get_current();
    out.sensors_data = m_sensors.dump();
    return out;
}