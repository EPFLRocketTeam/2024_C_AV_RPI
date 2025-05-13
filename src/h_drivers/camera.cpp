#include <iostream>
#include "camera.h"
#include "data.h"
#include "i2c_interface.h"
#include "intranet_commands.h"

Camera::Camera(const uint8_t address) : m_address(address) {
    try {
        I2CInterface::getInstance().open(m_address);
    }catch(const I2CInterfaceException& e) {
        std::cout << "Error during camera " << m_address << " initialization " << e.what() << "\n";
    }
}

Camera::~Camera() {
    try {
        I2CInterface::getInstance().close(m_address);
    }catch(const I2CInterfaceException& e) {
        std::cout << "Error during camera " << m_address << " deinitialization " << e.what() << "\n";
    }
}

void Camera::write_timestamp(const DataDump& dump) {
    const uint32_t timestamp(dump.av_timestamp);
    try {
        I2CInterface::getInstance().write(m_address, CAM_TIMESTAMP_MAIN, (uint8_t*)&timestamp, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("Camera write_timestamp error: ");
        throw CameraException(msg + e.what());
    }
}

bool Camera::read_is_recording() {
    //
}

void Camera::write_recording() {
    //
}

// TODO
void Camera::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    switch (dump.av_state) {
        case State::INIT:
        case State::CALIBRATION:
        case State::MANUAL:
        case State::ARMED:
        case State::READY:
        case State::THRUSTSEQUENCE:
        case State::LIFTOFF:
        case State::ASCENT:
        case State::DESCENT:
        case State::LANDED:
        case State::ERRORGROUND:
        case State::ERRORFLIGHT:
            break;
    }
}
