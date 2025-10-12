#ifndef TELECOM_H
#define TELECOM_H

#include <capsule.h>
#include <Protocol.h>
#include <exception>
#include <string>
#include "h_driver.h"

class Telecom : public HDriver {
public:
    Telecom();
    ~Telecom() = default;

    void check_policy(const DataDump& dump, const uint32_t delta_ms) override;

    bool begin();
    void send_telemetry();
    void reset_cmd();
    void update();

    static void handle_uplink(int packet_size);
    void handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint32_t len);
    static void handle_downlink(int packet_size);
    void handle_capsule_downlink(uint8_t packet_id, uint8_t* data_in, uint32_t len);
    static void handle_tx_done();

private:
    bool send_packet(uint8_t packet_id, uint8_t* data, uint32_t len);
    void restart_loras();

    Capsule<Telecom> capsule_uplink;
    Capsule<Telecom> capsule_downlink;

    av_uplink_t last_packet;
    bool new_cmd_received;
    uint32_t packet_number, last_packet_number;
    uint32_t telecom_restart_timer;
};

class TelecomException : public std::exception {
public:
    TelecomException(const std::string& msg_) : msg(msg_) {}

    virtual const char* what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
};

#endif /* TELECOM_H */
