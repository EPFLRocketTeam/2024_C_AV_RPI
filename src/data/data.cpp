#include "data.h"

Data::Data() {}

Data::~Data() {}

void Data::send() {
    const SensFiltered sens(m_sensors.get_clean());
    av_downlink_t packet;

    packet.gnss_lat = sens.position.lat;
    packet.gnss_lon = sens.position.lng;
    packet.gnss_alt = sens.position.alt;
    
    // We don't use a 2nd GPS
    packet.gnss_lat_r = 0.0f;
    packet.gnss_lon_r = 0.0f;
    packet.gnss_alt_r = 0.0f;

    packet.N2_pressure = sens.N2_pressure;
    packet.fuel_pressure = sens.fuel_pressure;
    packet.LOX_pressure = sens.LOX_pressure;
    packet.fuel_level = sens.fuel_level;
    packet.LOX_level = sens.LOX_level;
    packet.engine_temp = sens.engine_temperature;
    packet.igniter_pressure = sens.igniter_pressure;
    packet.LOX_inj_pressure = sens.LOX_inj_pressure;
    packet.fuel_inj_pressure = sens.fuel_inj_pressure;
    packet.chamber_pressure = sens.chamber_pressure;

    m_telecom.send_packet(CAPSULE_ID::AV_TELEMETRY, (uint8_t*)&packet, av_downlink_size);
}

bool Data::update() {
    m_telecom.update();
    updated = m_sensors.update();

    return updated;
}

DataDump Data::dump() const {
    return {m_telecom.get_cmd(), m_sensors.get_raw(), m_sensors.get_clean()};
}