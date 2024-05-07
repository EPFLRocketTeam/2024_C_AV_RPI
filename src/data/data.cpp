#include "../include/data/data.h"

Data::Data() {}

Data::~Data() {}

bool Data::update() {
    //m_telecom.update();
    updated = m_sensors.update();

    return updated;
}

// --------- By me ------------
float Data::get_time() {
    return time;
}

Sensors Data::get_sensors() {
    return m_sensors;
}

void Data::set_time(float t) {
    time = t;
}

// Status

void Data::set_adxl_status(uint8_t value) {
    m_sensors.set_adxl_status(value);
}

void Data::set_adxl_aux_status(uint8_t value) {
    m_sensors.set_adxl_aux_status(value);
}

void Data::set_bmi_accel_status(uint8_t value) {
    m_sensors.set_bmi_accel_status(value);
}

void Data::set_bmi_aux_accel_status(uint8_t value) {
    m_sensors.set_bmi_aux_accel_status(value);
}

void Data::set_bmi_gyro_status(uint8_t value) {
    m_sensors.set_bmi_gyro_status(value);
}

void Data::set_bmi_aux_gyro_status(uint8_t value) {
    m_sensors.set_bmi_aux_gyro_status(value);
}

void Data::set_bmp_status(bmp3_status value) {
    m_sensors.set_bmp_status(value);
}

void Data::set_bmp_aux_status(bmp3_status value) {
    m_sensors.set_bmp_aux_status(value);
}

// Values

void Data::set_adxl(adxl375_data value) {
    m_sensors.set_adxl(value);
}

void Data::set_adxl_aux(adxl375_data value) {
    m_sensors.set_adxl_aux(value);
}

void Data::set_bmi_accel(bmi08_sensor_data_f value) {
    m_sensors.set_bmi_accel(value);
}

void Data::set_bmi_aux_accel(bmi08_sensor_data_f value) {
    m_sensors.set_bmi_aux_accel(value);
}

void Data::set_bmi_gyro(bmi08_sensor_data_f value) {
    m_sensors.set_bmi_gyro(value);
}

void Data::set_bmi_aux_gyro(bmi08_sensor_data_f value) {
    m_sensors.set_bmi_aux_gyro(value);
}

void Data::set_bmp(bmp3_data value) {
    m_sensors.set_bmp(value);
}

void Data::set_bmp_aux(bmp3_data value) {
    m_sensors.set_bmp_aux(value);
}

void Data::set_altitude(double value) {
    m_sensors.set_altitude(value);
}

void Data::set_velocity(Vector3 value) {
    m_sensors.set_velocity(value);
}
// -----------------------------

DataDump Data::dump() const {
    return {/*m_telecom.get_cmd(),*/ m_sensors.dump()};
}