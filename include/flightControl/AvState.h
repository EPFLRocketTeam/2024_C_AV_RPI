// This is the header file for the AvState class, which is a part of the flightControl module.
// The AvState class is a finite state machine (FSM) that represents the state of the rocket.
// The class has a constructor and a destructor, as well as a number of functions that transition from one state to others possible states.

#include <iostream>
#include <list>
#include "data/thresholds.h"
#include <string>
#include "data.h"



#ifndef AVSTATE_H
#define AVSTATE_H


struct UPLink;

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


    void update(SensFiltered data, UPLink uplink, bool status);
    State* possibleStates();
    std::string stateToString(State state);

private:
    State fromIdle(SensFiltered data, UPLink uplink);
    State fromDescent(SensFiltered data, UPLink uplink);
    State fromAscent(SensFiltered data, UPLink uplink);
    State fromCalibration(SensFiltered data, UPLink uplink, bool status);
    State fromErrorGround(SensFiltered data, UPLink uplink);
    State fromErrorFlight();
    State fromThrustSequence(SensFiltered data, UPLink uplink);
    State fromManual(SensFiltered data, UPLink uplink);
    State fromArmed(SensFiltered data, UPLink uplink);

    State fromLanded();
    Thresholds thresholds;
    State currentState;
};

#endif
