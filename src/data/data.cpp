#include "data.h"

Data::Data() {}

Data::~Data() {}

bool Data::update() {
    m_telecom.update();
    updated = m_sensors.update();

    return updated;
}

DataDump Data::dump() const {
    return {m_telecom.get_cmd(), m_sensors.dump(), calibrated()};
}

bool Data::calibrated() const {
    SensStatus s = m_sensors.get_status();
    return s.adxl_status && s.adxl_aux_status && s.bmi_status && s.bmi_aux_status;
}