//
// Created by marin on 16.11.2024.
//

#ifndef FLIGHT_COMPUTER_DRIVERINTERFACE_H
#define FLIGHT_COMPUTER_DRIVERINTERFACE_H
#include "data.h"

class DriverInterface {
public :
    virtual void checkPolicy(DataDump dump) =0;
};

#endif //FLIGHT_COMPUTER_DRIVERINTERFACE_H
