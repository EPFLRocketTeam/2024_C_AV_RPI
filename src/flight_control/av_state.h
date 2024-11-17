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
#include "States_def.h"




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
    void update(DataDump dump);
    State* possibleStates();
    std::string stateToString(State state);

private:
    State fromInit(DataDump dump);
    State fromDescent(DataDump dump);
    State fromAscent(DataDump dump);
    State fromCalibration(DataDump dump);
    State fromErrorGround(DataDump dump);
    State fromErrorFlight(DataDump dump);
    State fromThrustSequence(DataDump dump);
    State fromManual(DataDump dump);
    State fromArmed(DataDump dump);
    State fromLanded(DataDump dump);
    State fromLiftoff(DataDump dump);
    State fromReady(DataDump dump);
    State currentState;
};

#endif
