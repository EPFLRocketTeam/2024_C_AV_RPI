//
// Created by marin on 17.11.2024.
//


#ifndef FLIGHT_COMPUTER_PROP_H
#define FLIGHT_COMPUTER_PROP_H

#include "../DriverInterface.h"

class Prop: public DriverInterface {

public:
    Prop();
    ~Prop() = default;

    void checkPolicy(DataDump dump) override;
};


#endif //FLIGHT_COMPUTER_PROP_H
