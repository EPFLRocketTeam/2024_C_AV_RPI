#include <iostream>
#include <cassert>
#include "camera.h"
#include "data.h"
#include "logger.h"
#include "i2c_interface.h"
#include "intranet_commands.h"

Camera::Camera(const uint8_t address) noexcept : m_address(address), m_recording(0) {
    switch (m_address) {
        case AV_NET_ADDR_CAM_SEP:
            m_id = "SEP";
            break;
        case AV_NET_ADDR_CAM_UP:
            m_id = "UP";
            break;
        case AV_NET_ADDR_CAM_DOWN:
            m_id = "DOWN";
            break;
    }
    try {
        I2CInterface::getInstance().open(m_address);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf(Logger::ERROR, "Error during camera %x driver initialization: %s", m_address, e.what());
    }
}

Camera::~Camera() {
    try {
        I2CInterface::getInstance().close(m_address);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf(Logger::ERROR, "Error during camera %x driver deinitialization: %s", m_address, e.what());
    }
}

void Camera::read_register(const uint8_t reg_addr, uint8_t* data) {
    try {
        I2CInterface::getInstance().read(m_address, reg_addr, data, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("Camera " + m_id + " I2C error: failed reading from register ");
        throw CameraException(msg + std::to_string(reg_addr) + "\n\t" + e.what());
    }
    assert(sizeof(data) <= AV_NET_XFER_SIZE);
}

void Camera::write_register(const uint8_t reg_addr, const uint8_t* value) {
    assert(sizeof(value) <= AV_NET_XFER_SIZE);
    try {
        I2CInterface::getInstance().write(m_address, reg_addr, value, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("Camera " + m_id + " I2C error: failed writing to register ");
        throw CameraException(msg + std::to_string(reg_addr) + "\n\t" + e.what());
    }
}

void Camera::write_timestamp(const DataDump& dump) {
    const uint32_t timestamp(dump.av_timestamp);
    write_register(AV_NET_CAM_TIMESTAMP, (uint8_t*)&timestamp);
}

bool Camera::read_is_recording() {
    uint32_t rslt(0);
    read_register(AV_NET_CAM_RECORDING, (uint8_t*)&rslt);
    m_recording = (rslt == AV_NET_CMD_ON);

    // Cameras status are stored in the same order as I2C addresses (SEP, UP, DOWN)
    uint8_t gr(Data::CAM_RECORDING_SEP + ((m_address - AV_NET_ADDR_CAM_SEP) >> 4));
    Data::get_instance().write((Data::GoatReg)gr, &m_recording);

    return m_recording;
}

void Camera::send_start_recording() {
    const uint32_t cmd(AV_NET_CMD_ON);
    write_register(AV_NET_CAM_RECORDING, (uint8_t*)&cmd);
}

void Camera::send_stop_recording() {
    const uint32_t cmd(AV_NET_CMD_OFF);
    write_register(AV_NET_CAM_RECORDING, (uint8_t*)&cmd);
}

// TODO
void Camera::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    this->delta_ms = delta_ms;
    count_ms += delta_ms;
    switch (dump.av_state) {
        case State::INIT:
        case State::CALIBRATION:
        case State::FILLING:
        case State::ARMED:
            break;
        case State::PRESSURIZATION:
            handle_pressurized();
            break;
        case State::IGNITION:
        case State::BURN:
        case State::ASCENT:
        case State::DESCENT:
        case State::LANDED:
        case State::ABORT_ON_GROUND:
        case State::ABORT_IN_FLIGHT:
            break;
    }
}

void Camera::handle_pressurized() {
    // Send 'start recording' command every 100ms until camera acks it's recording 
    if (count_ms >= 100 && !m_recording) {
        static unsigned attempts(0);
        send_start_recording();

        if (attempts >= 100) {
            // TODO: log error message and/or throw exception
        }

        read_is_recording();

        ++attempts;
        count_ms = 0;
    }
}

void Camera::handle_landed() {
    if (count_ms >= 500 && m_recording) {
        send_stop_recording();
        read_is_recording();

        count_ms = 0;
    }
}
