#ifndef TELECOM_INTERFACE_H
#define TELECOM_INTERFACE_H
#include <cstdint>

class Telecom_Interface {
public:
    virtual ~Telecom_Interface() noexcept = default;
    virtual bool begin() = 0;
    virtual void send_telemetry() = 0;
    virtual void update() = 0;
    virtual void reset_cmd() = 0;
    virtual void handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint32_t len) = 0;
    virtual void handle_capsule_downlink(uint8_t packet_id, uint8_t* data_in, uint32_t len) = 0;
};

#endif // TELECOM_INTERFACE_H