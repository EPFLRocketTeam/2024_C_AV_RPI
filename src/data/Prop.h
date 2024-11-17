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
private:
    void handleErrorFlight(DataDump dump);
    void killIgnitor();
    void killEngine();
    void ventLOX();
    void ventFuel();
    void ventN2();
};


#endif //FLIGHT_COMPUTER_PROP_H
