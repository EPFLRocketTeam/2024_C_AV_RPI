//
// Created by marin on 13/04/2024.
//

#ifndef FAKETELECOM_H
#define FAKETELECOM_H

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
#endif //FAKETELECOM_H
