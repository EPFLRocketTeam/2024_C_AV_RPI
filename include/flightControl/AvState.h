// This is the header file for the AvState class, which is a part of the flightControl module.
// The AvState class is a finite state machine (FSM) that represents the state of the rocket.
// The class has a constructor and a destructor, as well as a number of functions that transition from one state to others possible states.

#include <iostream>
#include <string>
#include "../data/data.h"


#ifndef AVSTATE_H
#define AVSTATE_H


enum class State
{
    IDLE,
    LANDED,
    DESCENT,
    ASCENT,
    CALIBRATION,
    ERRORGROUND,
    ERRORFLIGHT,
    THRUSTSEQUENCE,
    MANUAL,
    ARMED,
    READY
};

// Path: AV-Firehorn-Rpi/include/flightControl/AvState.h
// Compare this snippet from AV-Firehorn-Rpi/src/flightControl/FSM.cpp:
//     // this function allows to get the current state of the FSM
// functions that transition from one state to others possible states
class AvState
{
public:
    // constructor
    AvState(Data data);
    // destructor
    ~AvState();

    // this function allows to get the current state of the FSM
    State getCurrentState();
    
    void update(Data data);


    State *possibleStates();

    std::string stateToString(State state);
private:
    State currentState;
    Data data;
};




#endif
