#ifndef TELECOM_H
#define TELECOM_H

#include "capsule.h"
#include <PacketDefinition.h>
#include "data.h"

/**
 * @brief A struct made only for convenience. Holds exactly
 * the same data than av_uplink_t, but the command ID is already
 * cast from uint8_t to CMD_ID to facilitate switch statements.
 */
struct UplinkCmd {
    CMD_ID id;
    uint8_t value;
};

class Telecom {
public:
    Telecom(Data& data);


    ~Telecom() = default;
    bool begin();
    void send_packet(uint8_t packet_id, uint8_t* data, uint16_t len);
    void update();
    UplinkCmd get_cmd() const;
    void reset_cmd();

    static void handle_uplink(int packet_size);
    void handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint16_t len);
    static void handle_downlink(int packet_size);
    void handle_capsule_downlink(uint8_t packet_id, uint8_t* data_in, uint16_t len);

private:
    Capsule<Telecom> capsule_uplink;
    Capsule<Telecom> capsule_downlink;
    Data data;

    av_uplink_t last_packet;
    bool new_cmd_received;
};

#endif /* TELECOM_H */