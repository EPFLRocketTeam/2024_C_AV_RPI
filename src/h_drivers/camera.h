#ifndef CAMERA_H
#define CAMERA_H

#include <exception>
#include <string>
#include "h_driver.h"

struct DataDump;

class Camera : public HDriver {
public:
    Camera(const uint8_t address);
    virtual ~Camera();

    void check_policy(const DataDump& dump, const uint32_t delta_ms);

    void write_timestamp(const DataDump& dump);
    void send_wake_up();
    bool read_is_woken_up();
    bool read_is_recording();
    void write_recording();

private:
    uint8_t m_address;
};

class CameraException : public std::exception {
public:
    CameraException(const std::string& msg_) : msg(msg_) {}

    virtual const char* what() const throw() {
        return msg.c_str();
    }
private:
    std::string msg;
};

#endif /* CAMERAS_H */
