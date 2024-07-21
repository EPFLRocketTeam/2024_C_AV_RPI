#include "AvState.h"
#include <string.h>
#include <list>
#include <data/sensors.h>

#include "data/telecom.h"


AvState::AvState(const Thresholds& thresholds) : thresholds(thresholds)
{
    this->currentState = State::IDLE;
}

// destructor
AvState::~AvState()
{
    currentState = State::IDLE;
}


// this function allows to get the current state of the FSM
State AvState::getCurrentState()
{
    return currentState;
}


void sendError()
{
    //print in console the "error"
    std::cout << "Error" << std::endl;
}

bool error()
{
    return false;
}


State AvState::fromIdle(SensFiltered data, UPLink uplink)
{
    if ((uplink.id == CMD_ID::AV_CMD_CALIBRATE))
    {
        return State::CALIBRATION;
    }
    return State::IDLE;
}

State AvState::fromLanded() { return State::LANDED; }

State AvState::fromDescent(SensFiltered data, UPLink uplink)
{
    //norm of the speed vector
    if (uplink.id == CMD_ID::AV_CMD_ABORT || uplink.id == CMD_ID::AV_CMD_MANUAL_DEPLOY)
    {
        return State::ERRORFLIGHT;
    }
    double speed = (data.speed.x * data.speed.x + data.speed.y * data.speed.y + data.speed.z * data.speed.z);
    if (speed < 0)
    {
        return State::LANDED;
    }
    return State::DESCENT;
}

State AvState::fromAscent(SensFiltered data, UPLink uplink)
{
    if (uplink.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if (data.speed.z <= 0)
    {
        return State::DESCENT;
    }
    return State::ASCENT;
}

State AvState::fromCalibration(SensFiltered data, UPLink uplink, bool status)
{
    if (error() || uplink.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
        //check if the calibration is done
    }
    else if (status)
    {
        return State::MANUAL;
    }
    else
    {
        return State::CALIBRATION;
    }
}


State AvState::fromErrorGround(SensFiltered data, UPLink uplink)
{
    if (uplink.id == CMD_ID::AV_CMD_RECOVER)
    {
        return State::IDLE;
    }
}

State AvState::fromErrorFlight()
{
    return State::ERRORFLIGHT;
}

State AvState::fromThrustSequence(SensFiltered data, UPLink uplink)
{
    double speed = (data.speed.x * data.speed.x + data.speed.y * data.speed.y + data.speed.z * data.speed.z);
    if (uplink.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if (data.ignited)
    {
        return State::ASCENT;
    }
    else
    {
        return State::THRUSTSEQUENCE;
    }
}

State AvState::fromManual(SensFiltered data, UPLink uplink)
{
    if (error() || uplink.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
    }
        //TODO: recheck i threholds are the wanted ones
    else if (uplink.id == CMD_ID::AV_CMD_ARM &&
             data.fuel_pressure > thresholds.fuel_pressure_wanted &&
             data.LOX_pressure > thresholds.lox_pressure_wanted &&
             data.chamber_pressure > thresholds.chamber_pressure_wanted &&
             data.engine_temperature > thresholds.engine_temp_zero &&
             data.fuel_inj_pressure > thresholds.fuel_pressure_wanted &&
             data.N2_pressure > thresholds.n2_pressure_zero &&
             data.LOX_level > thresholds.lox_level_zero &&
             data.fuel_level > thresholds.fuel_level_zero)
    {
        return State::ARMED;
    }
    {
        return State::ARMED;
    }
    return State::MANUAL;
}

State AvState::fromArmed(SensFiltered data, UPLink uplink)
{
    if (error())
    {
        return State::ERRORGROUND;
    }
    else
    {
        switch (uplink.id)
        {
            case CMD_ID::AV_CMD_IGNITION:
                if (data.igniter_pressure >= thresholds.igniter_pressure_wanted &&
                    data.chamber_pressure >= thresholds.chamber_pressure_wanted )
                {
                    return State::THRUSTSEQUENCE;

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

void AvState::update(SensFiltered data, UPLink uplink, bool status)

{
    switch (currentState)
    {
        case State::IDLE:
            this->currentState = fromIdle(data, uplink);
            break;
        case State::LANDED:
            currentState = fromLanded();
        case State::DESCENT:
            currentState = fromDescent(data, uplink);
            break;
        case State::ASCENT:
            currentState = fromAscent(data, uplink);
            break;
        case State::CALIBRATION:
            currentState = fromCalibration(data, uplink, status);
            break;
        case State::ERRORGROUND:
            currentState = fromErrorGround(data, uplink);
            break;
        case State::ERRORFLIGHT:
            currentState = fromErrorFlight();
            break;
        case State::THRUSTSEQUENCE:
            currentState = fromThrustSequence(data, uplink);
            break;
        case State::MANUAL:
            currentState = fromManual(data, uplink);
            break;
        case State::ARMED:
            currentState = fromArmed(data, uplink);
            break;
        default:
            currentState = State::ERRORFLIGHT;
    }
}
std::string AvState::stateToString(State state)
{
    switch (state)
    {
        case State::IDLE:
            return "IDLE";
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



