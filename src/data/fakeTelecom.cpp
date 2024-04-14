//
// Created by marin on 13/04/2024.
//

#include "../include/data/fakeTelecom.h"

#include <string>

FakeTelecom::FakeTelecom() : new_cmd_received(false)
{
}

FakeTelecom::~FakeTelecom()
{
}

void FakeTelecom::update(std::string data)
{
    // Parse the data
    //delimiters are ','
    //we start from the back of the string
//we get the last 2 numbers

    last_packet.order_id = data[data.size() - 2] - '0';
    last_packet.order_value = data[data.size() - 1] - '0';
    new_cmd_received = true;



}

UPLink FakeTelecom::get_cmd() const
{
    UPLink out;
    out.id = static_cast<CMD_ID>(last_packet.order_id);
    out.value = last_packet.order_value;
    return out;
}

void FakeTelecom::reset_cmd()
{
    last_packet = {0, 0};
}

// Path: src/data/fakeSensors.cpp