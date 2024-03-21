#ifndef TELECOM_H
#define TELECOM_H

#include <capsule.h>
#include <ERT_RF_Protocol_Interface/PacketDefinition.h>

struct UPLink {
    CMD_ID id;
    uint8_t value;
};

class Telecom {
public:
    Telecom();
    ~Telecom();

    void update();
    UPLink get_cmd() const;
    void reset_cmd();
private:
    av_uplink_t last_packet;
    bool new_cmd_received;
};

class FakeTelecom : public Telecom {
public:
    FakeTelecom();
    ~FakeTelecom();

    void set_cmd(CMD_ID id, uint8_t value);
    void reset_cmd();
    UPLink get_cmd() const;
private:
    UPLink cmd;

};

#endif /* TELECOM_H */