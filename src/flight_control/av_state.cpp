#include <Protocol.h>
#include "av_state.h"
#include "sensors.h"
#include "data.h"


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


//TODO Check if we need this function
/*bool error()
{
    return false;
}*/


State AvState::fromInit(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_CALIBRATE)
    {
        return State::CALIBRATION;
    }
    return this->currentState;
}

State AvState::fromLanded(DataDump dump) {
    return this->currentState;
}

State AvState::fromDescent(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT || dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_MANUAL_DEPLOY)
    {
        return State::ERRORFLIGHT;
    }
    //TODO injection/igniter pressure 0 
    
    else if (dump.nav.speed.norm() < SPEED_ZERO && dump.depressurised())
    {
        return State::LANDED;
    }
    return this->currentState;
}

State AvState::fromAscent(DataDump dump)
{
    if (dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if (dump.nav.speed.z < SPEED_ZERO)
    {
        return State::DESCENT;
    }
    return this->currentState;
}

State AvState::fromCalibration(DataDump dump)
{
    // If the sensors are not detected or the radio signal is lost we go to the ERRORGROUND state
    // TODO: add the right checks
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
    }
    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_RECOVER)
    {
        return State::INIT;
    }
    // If all the sensors are calibrated and ready for use we go to the MANUAL state
    else if (dump.event.calibrated)
    {
        return State::MANUAL;
    }
    return this->currentState;
}


State AvState::fromErrorGround(DataDump dump)
{
    if (dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_RECOVER)
    {
        return State::INIT;
    }
    return  this->currentState;
}

State AvState::fromErrorFlight(DataDump dump)
{
    return this->currentState;
}

State AvState::fromThrustSequence(DataDump dump)
{
    if (dump.telemetry_cmd.id== CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }

    // If the pression is too low in the igniter or combustion chamber we go to the ARMED state
    // a bit agressive TODO: have  a counter or a sleep
    //TODO: check FAILEDIGNIT
    else if (dump.event.ignition_failed == true)
    {
        return State::ARMED;
    }

    // If the engine is properly ignited and a liftoff has been detected we go to LIFTOFF state
    // TODO: ensure those are the right checks
    else if (dump.nav.speed.z > SPEED_ZERO && dump.nav.altitude > ALTITUDE_ZERO && dump.event.ignited)
    {
        return State::LIFTOFF;
    }

    return this->currentState;
}


State AvState::fromManual(DataDump dump)
{
    //check all thresholds individually
    //TODO: recheck if threholds are the wanted ones
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ARM)
    {
        return State::ARMED;
    }
    return State::MANUAL;
}

State AvState::fromArmed(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORGROUND;
    }
    // If the propulsion is OK we go to the READY state
    // TODO: ensure those are the right checks
    else if (dump.event.armed)
    {
        return State::READY;
    }
    return this->currentState;
}

State AvState::fromReady(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_IGNITION)
    {
        return State::THRUSTSEQUENCE;
    }
    return this->currentState;
}

State AvState::fromLiftoff(DataDump dump)
{

    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    // If the altitude threashold is cleared we go to the ASCENT state
    // TODO: ensure those are the right checks
    else if (dump.nav.altitude > ALTITUDE_THRESHOLD)
    {
        return State::ASCENT;
    }
    return this->currentState;
}

void AvState::update(DataDump dump)
{
    switch (currentState)
    {
        case State::INIT:
            currentState = fromInit(dump);
            break;
        case State::LANDED:
            currentState = fromLanded(dump);
        case State::DESCENT:
            currentState = fromDescent(dump);
            break;
        case State::READY:
            currentState = fromReady(dump);
            break;
        case State::LIFTOFF:
            currentState = fromLiftoff(dump);
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
        case State::READY:
            return "READY";
            break;
        case State::LIFTOFF:
            return "LIFTOFF";
            break;
        default:
            return "ERROR";
            break;
    }
}



