//
// Created by marin on 13/04/2024.
//

#include "../include/data/fakeTelecom.h"

#include <cstring>
#include <string>

FakeTelecom::FakeTelecom()
{
    new_cmd_received = false;
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

    std::string delimiter = ",";
    size_t pos;
    std::string token;
    //list of all the values
    std::string values[16];
    std::string copy = data;

    for (int i = 0; i < 16; i++)
    {
        pos = data.find(delimiter);
        token = data.substr(0, pos);
        copy.erase(0, pos + 1);

        //only the last 2 values are interesting

        switch (i)
        {
            case 14:
                //check if empty
                    if (token.empty() && token == "nan"  )
                    {
                        last_packet.order_id = -1;
                        new_cmd_received = false;
                    }
                    else
                    {
                        last_packet.order_id = std::stoi(token);
                        new_cmd_received = true;
                    }
                break;
            case 15:
                last_packet.order_value = std::stoi(token);
                break;
            default:
                break;
        }



    }

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