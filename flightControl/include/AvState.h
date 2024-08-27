// This is the header file for the AvState class, which is a part of the flightControl module.
// The AvState class is a finite state machine (FSM) that represents the state of the rocket.
// The class has a constructor and a destructor, as well as a number of functions that transition from one state to others possible states.

#ifndef AVSTATE_H
#define AVSTATE_H

#include <iostream>
#include <list>
#include <string>
#include "PacketDefinition.h"
#include "thresholds.h"

enum class State
{
    INIT,
    LANDED,
    DESCENT,
    ASCENT,
    CALIBRATION,
    ERRORGROUND,
    ERRORFLIGHT,
    THRUSTSEQUENCE,
    MANUAL,
    ARMED
};


// Path: AV-Firehorn-Rpi/include/flightControl/AvState.h
// Compare this snippet from AV-Firehorn-Rpi/src/flightControl/FSM.cpp:
//     // this function allows to get the current state of the FSM
// functions that transition from one state to others possible states
class AvState
{
public:
    // constructor
    explicit AvState(const Thresholds& thresholds);
    // destructor
    ~AvState();

    // this function allows to get the current state of the FSM
    State getCurrentState();


    void update();
    State* possibleStates();
    std::string stateToString(State state);

private:
    State fromInit();
    State fromDescent();
    State fromAscent();
    State fromCalibration();
    State fromErrorGround();
    State fromErrorFlight();
    State fromThrustSequence();
    State fromManual();
    State fromArmed();

    State fromLanded();
    Thresholds thresholds;
    State currentState;
};

#endif
