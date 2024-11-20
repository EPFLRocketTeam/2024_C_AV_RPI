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
    void listenToValves(DataDump dump);
    void safetyChecks(DataDump dump);
    void ignition(DataDump dump); // fixme: should ignite event be here?
    void pressureChecks(DataDump dump);
    void killIgnitor();
    void startEngine();
    void killEngine();
    void ventLOX();
    void ventFuel();
    void ventN2();
    void valveIgniterLOX();
    void valveMainLOX();
    void valveVentLOX();
    void valveIgniterEthanol();
    void valveMainEthanol();
    void valveVentEthanol();
};


#endif //FLIGHT_COMPUTER_PROP_H
