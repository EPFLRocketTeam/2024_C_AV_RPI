#include "AvState.h"
#include <string.h>
#include <list>
#include <sensors.h>
#include "telecom.h"
#include "data.h"
#include "PacketDefinition.h"


AvState::AvState(const Thresholds& thresholds) : thresholds(thresholds)
{
    this->currentState = State::INIT;
}

// destructor
AvState::~AvState()
{
    // nothing to do
}


// this function allows to get the current state of the FSM
State AvState::getCurrentState()
{
    return currentState;
}


//TODO not implemented, necessity to decide where errors come from
bool error()
{
    return false;
}


State AvState::fromInit(DataDump data)
{
    if (data.telecom_status.id == CMD_ID::AV_CMD_CALIBRATE)
    {
        return State::CALIBRATION;
    }
    return State::INIT;
}

State AvState::fromLanded() { return State::LANDED; }

State AvState::fromDescent(DataDump data)
{
    //norm of the speed vector
    if (data.telecom_status.id == CMD_ID::AV_CMD_ABORT || data.telecom_status.id == CMD_ID::AV_CMD_MANUAL_DEPLOY)
    {
        return State::ERRORFLIGHT;
    }
    double speed = (data.sensors_data.speed.x * data.sensors_data.speed.x + data.sensors_data.speed.y * data.sensors_data.speed.y + data.sensors_data.speed.z * data.sensors_data.speed.z);
    if (speed < 0)
    {
        return State::LANDED;
    }
    return State::DESCENT;
}

State AvState::fromAscent(DataDump data)
{
    if (data.telecom_status.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if (data.sensors_data.speed.z <= 0)
    {
        return State::DESCENT;
    }
    return State::ASCENT;
}

State AvState::fromCalibration(DataDump data)
{
    if (error() || data.telecom_status.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
        //check if the calibration is done
    }
    else
    {
        return State::CALIBRATION;
    }
}


State AvState::fromErrorGround(DataDump data)
{
    if (data.telecom_status.id == CMD_ID::AV_CMD_RECOVER)
    {
        return State::INIT;
    }
}

State AvState::fromErrorFlight()
{
    return State::ERRORFLIGHT;
}

State AvState::fromThrustSequence(DataDump data)
{
    double speed = (data.sensors_data.speed.x * data.sensors_data.speed.x + data.sensors_data.speed.y * data.sensors_data.speed.y + data.sensors_data.speed.z * data.sensors_data.speed.z);
    if (data.telecom_status.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if (data.sensors_data.speed.z>0) //TODO we need a ignite bool var
                                //given by the prop board
    {
        return State::ASCENT;
    }
    else
    {
        return State::THRUSTSEQUENCE;
    }
}


State AvState::fromManual(DataDump data)
{
    if (error() || data.telecom_status.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
    }
    //check all thresholds individually
        //TODO: recheck i threholds are the wanted ones
    else if (data.telecom_status.id == CMD_ID::AV_CMD_ARM)
    {
        return State::ARMED;
    }
    return State::MANUAL;
}

State AvState::fromArmed(DataDump data)
{
    if (error())
    {
        return State::ERRORGROUND;
    }
    else
    {
        switch (data.telecom_status.id)
        {
            case CMD_ID::AV_CMD_IGNITION:
                if (data.sensors_data.igniter_pressure >= thresholds.igniter_pressure_wanted)
                {
                    //possible log
                    if(data.sensors_data.chamber_pressure >= thresholds.chamber_pressure_wanted ) {
                        //possible log
                        return State::THRUSTSEQUENCE;
                    }
                    return State::ARMED;
                }else {
                    return State::ARMED;
                }
                break;
            case CMD_ID::AV_CMD_ABORT:
                return State::ERRORGROUND;
            default:
                State::ARMED;
        }
    }
}

State* possibleStates();

void AvState::update(DataDump data)

{
    switch (currentState)
    {
        case State::INIT:
            this->currentState = fromInit(data);
            break;
        case State::LANDED:
            currentState = fromLanded();
        case State::DESCENT:
            currentState = fromDescent(data);
            break;
        case State::ASCENT:
            currentState = fromAscent(data);
            break;
        case State::CALIBRATION:
            currentState = fromCalibration(data);
            break;
        case State::ERRORGROUND:
            currentState = fromErrorGround(data);
            break;
        case State::ERRORFLIGHT:
            currentState = fromErrorFlight();
            break;
        case State::THRUSTSEQUENCE:
            currentState = fromThrustSequence(data);
            break;
        case State::MANUAL:
            currentState = fromManual(data);
            break;
        case State::ARMED:
            currentState = fromArmed(data);
            break;
        default:
            currentState = State::ERRORFLIGHT;
    }
}
std::string AvState::stateToString(State state)
{
    switch (state)
    {
        case State::INIT:
            return "INIT";
            break;
        case State::LANDED:
            return "LANDED";
            break;
        case State::DESCENT:
            return "DESCENT";
            break;
        case State::ASCENT:
            return "ASCENT";
            break;
        case State::CALIBRATION:
            return "CALIBRATION";
            break;
        case State::ERRORGROUND:
            return "ERRORGROUND";
            break;
        case State::ERRORFLIGHT:
            return "ERRORFLIGHT";
            break;
        case State::THRUSTSEQUENCE:
            return "THRUSTSEQUENCE";
            break;
        case State::MANUAL:
            return "MANUAL";
            break;
        case State::ARMED:
            return "ARMED";
            break;
        default:
            return "ERROR";
            break;
    }
}



