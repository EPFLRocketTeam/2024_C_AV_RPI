#include "../../include/flightControl/AvState.h"
#include <string.h>
#include <list>
#include <data/fakeSensors.h>

#include "data/fakeTelecom.h"




AvState::AvState(Thresholds thresholds)

{
    this->currentState = State::IDLE;
    //initialize the thresholds
    this->thresholds = thresholds;

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

std::string telemetry_set[] = {"calibrate", "check", "ready", "launch", "abort", "arme"};

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
    double speed = (data.speed.x * data.speed.x + data.speed.y * data.speed.y + data.speed.z * data.speed.z);
    if (speed < 0)
    {
        return State::LANDED;
    }
    return State::DESCENT;
}

State AvState::fromAscent(SensFiltered data, UPLink uplink)
{
    if (data.speed.z <= 0)
    {
        return State::DESCENT;
    }
    return State::ASCENT;
}

State AvState::fromCalibration(SensFiltered data, UPLink uplink)
{
    if (error())
    {
        return State::ERRORGROUND;
    }
    else
    {
        switch (uplink.id)
        {
            case CMD_ID::AV_CMD_ABORT:
                return State::ERRORGROUND;
        case CMD_ID::AV_CMD_MANUAL_DEPLOY:
            return State::MANUAL;
        default: return State::CALIBRATION;
        }

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
    else if (speed < thresholds.speed_zero)
    {
        return State::LANDED;
    }
    {
        return State::ASCENT;
    }
    return State::THRUSTSEQUENCE;
}

State AvState::fromManual(SensFiltered data, UPLink uplink)
{
    if (error() || uplink.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
    }
    else if (uplink.id == CMD_ID::AV_CMD_ARM && data.fuel_pressure > thresholds.pressure_wanted)
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
    }else
    {
        switch (uplink.id)
        {
        case CMD_ID::AV_CMD_IGNITION:
            return State::THRUSTSEQUENCE;
        case CMD_ID::AV_CMD_ABORT:
            return State::ERRORGROUND;
        default:
            State::ARMED;
        }
    }
}

State* possibleStates();




void AvState::update(SensFiltered data, UPLink uplink)

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
        currentState = fromCalibration(data, uplink);
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
    case State::READY:
        return "READY";
        break;
    default:
        return "ERROR";
        break;
    }

}


