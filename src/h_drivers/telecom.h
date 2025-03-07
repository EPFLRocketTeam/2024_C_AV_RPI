#ifndef TELECOM_H
#define TELECOM_H

#include <capsule.h>
#include <Protocol.h>
#include "data.h"
#include "h_driver.h"

class Telecom : public HDriver {
public:
    Telecom();
    ~Telecom() = default;

    void check_policy(Data::GoatReg reg, const DataDump& dump) override;

    bool begin();
    void send_telemetry();
    void update();
    void reset_cmd();

    static void handle_uplink(int packet_size);
    void handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint32_t len);
    static void handle_downlink(int packet_size);
    void handle_capsule_downlink(uint8_t packet_id, uint8_t* data_in, uint32_t len);

private:
    void send_packet(uint8_t packet_id, uint8_t* data, uint32_t len);

    Capsule<Telecom> capsule_uplink;
    Capsule<Telecom> capsule_downlink;

    av_uplink_t last_packet;
    bool new_cmd_received;
};

#endif /* TELECOM_H */