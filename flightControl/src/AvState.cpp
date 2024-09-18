#include "AvState.h"
#include "sensors.h"
#include "data.h"
#include "Protocol.h"


AvState::AvState()
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


State AvState::fromInit(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_CALIBRATE)
    {
        return State::CALIBRATION;
    }
    return State::INIT;
}

State AvState::fromLanded(DataDump dump) {
    return State::LANDED; 
}

State AvState::fromDescent(DataDump dump)
{
    //norm of the speed vector
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT || dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_MANUAL_DEPLOY)
    {
        return State::ERRORFLIGHT;
    }


    if ( dump.nav.speed.z < SPEED_ZERO)
    {
        return State::LANDED;
    }
    return State::DESCENT;
}

State AvState::fromAscent(DataDump dump)
{
    if ( dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if ( dump.nav.accel.z < ACCEL_ZERO)
    {
        return State::DESCENT;
    }
    return State::ASCENT;
}

State AvState::fromCalibration(DataDump dump)
{
    if (error() || dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
        //check if the calibration is done
    }
    else
    {
        return State::CALIBRATION;
    }
}


State AvState::fromErrorGround(DataDump dump)
{
    if (dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_RECOVER)
    {
        return State::INIT;
    }
}

State AvState::fromErrorFlight(DataDump dump)
{
    return State::ERRORFLIGHT;
}

State AvState::fromThrustSequence(DataDump dump)
{
    if (dump.telemetry_cmd.id== CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if (dump.nav.speed.z > SPEED_ZERO)
    {
        return State::ASCENT;
    }
    else
    {
        return State::THRUSTSEQUENCE;
    }
}


State AvState::fromManual(DataDump dump)
{
    if (error() || dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
    }
    //check all thresholds individually
        //TODO: recheck i threholds are the wanted ones
    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ARM)
    {
        return State::ARMED;
    }
    return State::MANUAL;
}

State AvState::fromArmed(DataDump dump)
{
    if (error())
    {
        return State::ERRORGROUND;
    }
    else
    {
        switch (dump.telemetry_cmd.id)
        {
            case CMD_ID::AV_CMD_IGNITION:
                if (dump.prop.fuel_inj_pressure >= IGNITER_PRESSURE_WANTED)
                {
                    //possible log
                    if( dump.prop.chamber_pressure >= CHAMBER_PRESSURE_WANTED ) {
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
                return State::ARMED;
        }
    }
}


void AvState::update(DataDump dump)
{
    switch (currentState)
    {
        case State::INIT:
            this->currentState = fromInit(dump);
            break;
        case State::LANDED:
            currentState = fromLanded(dump);
        case State::DESCENT:
            currentState = fromDescent(dump);
            break;
        case State::ASCENT:
            currentState = fromAscent(dump);
            break;
        case State::CALIBRATION:
            currentState = fromCalibration(dump);
            break;
        case State::ERRORGROUND:
            currentState = fromErrorGround(dump);
            break;
        case State::ERRORFLIGHT:
            currentState = fromErrorFlight(dump);
            break;
        case State::THRUSTSEQUENCE:
            currentState = fromThrustSequence(dump);
            break;
        case State::MANUAL:
            currentState = fromManual(dump);
            break;
        case State::ARMED:
            currentState = fromArmed(dump);
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



