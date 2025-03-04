// This is the header file for the AvState class, which is a part of the flightControl module.
// The AvState class is a finite state machine (FSM) that represents the state of the rocket.
// The class has a constructor and a destructor, as well as a number of functions that transition from one state to others possible states.

#ifndef AVSTATE_H
#define AVSTATE_H

#include <iostream>
#include <list>
#include <string>
#include "thresholds.h"
#include "data.h"

// Path: AV-Firehorn-Rpi/include/flightControl/AvState.h
// Compare this snippet from AV-Firehorn-Rpi/src/flightControl/FSM.cpp:
//     // this function allows to get the current state of the FSM
// functions that transition from one state to others possible states
class AvState
{
public:
    // constructor
    explicit AvState();
    // destructor
    ~AvState();
    // this function allows to get the current state of the FSM
    State getCurrentState();
    void update(DataDump &dump);
    std::string stateToString(State state);

private:
    State fromInit(DataDump const &dump);
    State fromDescent(DataDump const &dump);
    State fromAscent(DataDump const &dump);
    State fromCalibration(DataDump const &dump);
    State fromErrorGround(DataDump const &dump);
    State fromErrorFlight(DataDump const &dump);
    State fromThrustSequence(DataDump const &dump);
    State fromManual(DataDump const &dump);
    State fromArmed(DataDump const &dump);
    State fromLanded(DataDump const &dump);
    State fromLiftoff(DataDump const &dump);
    State fromReady(DataDump const &dump);
    State currentState;
};

#endif
