//
// Created by marin on 30/04/2024.
//

#ifndef FAKETELECOM_H
#define FAKETELECOM_H

#include "telecom.h"

class FakeTelecom : public Telecom
{
public:
    FakeTelecom();
    ~FakeTelecom();
    void set_update(std::string update);
    virtual void update() override;
    void set_cmd(CMD_ID id, uint8_t value);
    private:
     av_uplink_t last_packet;
    bool new_cmd_received;

};

#endif //FAKETELECOM_H
