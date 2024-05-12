#ifndef TELECOM_H
#define TELECOM_H

#include <capsule.h>
#include <ERT_RF_Protocol_Interface/PacketDefinition.h>
#include <string>
#include <gmock/gmock.h>

struct UPLink {
    CMD_ID id;
    uint8_t value;
};

class Telecom {
public:
    Telecom();
    virtual ~Telecom() = default;

    virtual void update();
    virtual UPLink get_cmd() const;
    virtual void reset_cmd();
protected:
    av_uplink_t last_packet;
    bool new_cmd_received;
};

class MockTelecom : public Telecom {
public:
    MOCK_METHOD(void, update, (), (override));
    MOCK_METHOD(UPLink, get_cmd, (), (const, override));
    MOCK_METHOD(void, reset_cmd, (), (override));
};



#endif /* TELECOM_H */