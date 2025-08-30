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
    void update(const DataDump &dump);
    std::string stateToString(State state);

private:
    State from_init(DataDump const &dump);
    State from_calibration(DataDump const &dump);
    State from_filling(DataDump const &dump);
    State from_armed(DataDump const &dump);
    State from_pressurized(DataDump const &dump);
    State from_abort_ground(DataDump const &dump);
    State from_ignition(DataDump const &dump);
    State from_landed(DataDump const &dump);
    State from_burn(DataDump const &dump);
    State from_ascent(DataDump const &dump);
    State from_descent(DataDump const &dump);
    State from_abort_flight(DataDump const &dump);
    State currentState;
};

#endif
