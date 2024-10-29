#include <Protocol.h>
#include "av_state.h"
#include "sensors.h"
#include "data.h"
#include "missing_sensors.h"
#include "PacketDefinition_Firehorn.h" // TODO: remove


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
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_MANUAL_DEPLOY)
    {
        return State::ERRORFLIGHT;
    }

    //TODO check when we can vent (not defined) 2nd stage RE

    // If the vehicule is immobile and fully depressurized we go to the LANDED state
    //TODO loxpressur == zero same for fuel @cleo
    if (VEHICULE_PRESSURE == VEHICULE_DEPRESSURIZED && 
    dump.nav.speed.x == 0 && dump.nav.speed.y == 0 && dump.nav.speed.z == 0)
    {
        return State::LANDED;
    }
    return State::DESCENT;
}

State AvState::fromAscent(DataDump dump)
{

    if (dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    // If the apogee is detected we move to the DESCENT state
    else if (dump.nav.accel.z < ACCEL_ZERO)
    {
        return State::DESCENT;
    }
    return State::ASCENT;
}
uint8_t check_status(DataDump dump){
    //FIXME false way of checking status 
    return dump.stat.adxl_status && dump.stat.adxl_aux_status && dump.stat.bmi_accel_status && dump.stat.bmi_aux_accel_status && dump.stat.bmi_gyro_status && dump.stat.bmi_aux_gyro_status;
}

State AvState::fromCalibration(DataDump dump)
{
    // If the sensors are not detected or the radio signal is lost we go to the ERRORGROUND state
    // TODO: add the right checks
    if (check_status(dump))
    {
        return State::ERRORGROUND;
    }
    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_RECOVER)
    {
        return State::INIT;
    }
    // If all the sensors are calibrated and ready for use we go to the MANUAL state
    // TODO: check whether this is the right way to do 
    else if (dump.stat.adxl_status && dump.stat.adxl_aux_status
    && dump.stat.bmi_accel_status && dump.stat.bmi_aux_accel_status
    && dump.stat.bmi_gyro_status && dump.stat.bmi_aux_gyro_status)
    {
        return State::MANUAL;
    }
    return State::CALIBRATION;
}


State AvState::fromErrorGround(DataDump dump)
{
    if (dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_RECOVER)
    {
        return State::INIT;
    }
    return  State::ERRORGROUND;
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
    // If the engine is properly ignited and a liftoff has been detected we go to LIFTOFF state 
    // TODO: ensure those are the right checks 
    else if (ENGINE_IGNITION == ENGINE_IGNITED || dump.nav.speed.z > SPEED_ZERO)
    {
        return State::LIFTOFF;
    }
    // If the pression is too low in the igniter or combustion chamber we go to the ARMED state
    else if (dump.prop.igniter_pressure < IGNITER_PRESSURE_WANTED || dump.prop.chamber_pressure < CHAMBER_PRESSURE_WANTED)
    {
        return State::ARMED;
    }
    return State::THRUSTSEQUENCE;
}

State AvState::fromManual(DataDump dump)
{
    // If the safety checks (valves open, vents open, no pressure) are failed we go to the ERRORGROUND state
    // TODO: ensure those are the right checks
    if (VALVES == VALVES_OPEN || VENTS == VENTS_OPEN || VEHICULE_PRESSURE == VEHICULE_DEPRESSURIZED) 
    {
        return State::ERRORGROUND;
    }
    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ARM)
    {
        return State::ARMED;
    }
    return State::MANUAL;
}

State AvState::fromArmed(DataDump dump)
{
    // TODO: ensure those are the right checks
    // If the safety checks (valves open, vents open, no pressure) are failed we go to the ERRORGROUND state
    if (VALVES == VALVES_OPEN || VENTS == VENTS_OPEN || VEHICULE_PRESSURE == VEHICULE_DEPRESSURIZED) 
    {
        return State::ERRORGROUND;
    }
    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_IGNITION)
    {
        return State::THRUSTSEQUENCE;
    }
    return State::ARMED;        
}

State AvState::fromLiftoff(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT) 
    {
        return State::ERRORFLIGHT;
    }
    // If the LV has cleared the minimum altitude threshold we go to the ASCENT state
    else if (dump.nav.altitude > ALTITUDE_MIN_WANTED) 
    {
        return State::ASCENT;
    }
    return State::LIFTOFF;

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
