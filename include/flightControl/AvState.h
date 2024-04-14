// This is the header file for the AvState class, which is a part of the flightControl module.
// The AvState class is a finite state machine (FSM) that represents the state of the rocket.
// The class has a constructor and a destructor, as well as a number of functions that transition from one state to others possible states.

#include <iostream>
#include <string>
#include <data/fakeSensors.h>

#include "AvData.h"


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
    ARMED,
    READY
};

class Thresholds
{
    // a class that contains the thresholds for the different data values
    // this class is used to hold the thresholds for the different data values

    Thresholds(const double speed_zero, const double speed_max, const double altitude_max, const double acceleration_max, const double pressure_max, const double pressure_wanted, const double pressure_min)
    {
        this->speed_zero = speed_zero;
        this->speed_max = speed_max;
        this->altitude_max = altitude_max;
        this->acceleration_max = acceleration_max;
        this->pressure_max = pressure_max;
        this->pressure_wanted = pressure_wanted;
        this->pressure_min = pressure_min;
    }

public:
    double speed_zero;
    double speed_max;
    double altitude_max;
    double acceleration_max;
    double pressure_max;
    double pressure_wanted;
    double pressure_min;
};

// Path: AV-Firehorn-Rpi/include/flightControl/AvState.h
// Compare this snippet from AV-Firehorn-Rpi/src/flightControl/FSM.cpp:
//     // this function allows to get the current state of the FSM
// functions that transition from one state to others possible states
class AvState
{
public:
    // constructor
    AvState(Thresholds thresholds);
    // destructor
    ~AvState();

    // this function allows to get the current state of the FSM
    State getCurrentState();

    
    void update(SensFiltered data, UPLink uplink);
    State *possibleStates();
    std::string stateToString(State state);



private:
    State fromIdle(SensFiltered data, UPLink uplink);
    State fromDescent(SensFiltered data, UPLink uplink);
    State fromAscent(SensFiltered data, UPLink uplink);
    State fromCalibration(SensFiltered data, UPLink uplink);
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
