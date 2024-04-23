#ifndef TELECOM_H
#define TELECOM_H

#include <capsule.h>
#include <ERT_RF_Protocol_Interface/PacketDefinition.h>

struct UpLink {
    CMD_ID id;
    uint8_t value;
};

class Telecom {
public:
    Telecom();
    ~Telecom();

    void begin();
    void update();
    UpLink get_cmd() const;
    void reset_cmd();
private:
    av_uplink_t last_packet;
    bool new_cmd_received;
};

#endif /* TELECOM_H */