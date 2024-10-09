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
    // TODO: should the first condition in the following 'or' be removed ?
    // TODO: should we add a if(error()) check ?
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT || dump.telemetry_cmd.id == CMD_ID::AV_CMD_MANUAL_DEPLOY)
    {
        return State::ERRORFLIGHT;
    }

    // TODO: shouldn't we also check that the vehicule is fully depressurized ?
    if (dump.nav.speed.z < SPEED_ZERO)
    {
        return State::LANDED;
    }
    return State::DESCENT;
}

State AvState::fromAscent(DataDump dump)
{
    if (error() || dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_ABORT)
    {
        return State::ERRORFLIGHT;
    }
    else if (dump.nav.accel.z < ACCEL_ZERO)
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
    }
    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_RECOVER)
    {
        return State::INIT
    }
    // If all the sensors are calibrated and ready for use we go to the Manual state
    // TODO: check whether this is the right way to do 
    else if (dump.stat.adxl_status && dump.stat.adxl_aux_status
    && dump.stat.bmi_accel_status && dump.stat.bmi_aux_accel_status
    && dump.stat.bmi_gyro_status && dump.stat.bmi_aux_gyro_status)
    {
        return State::MANUAL
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
    // TODO: ensure that the following checks that the engine is properly ignited
    // and a liftoff has been detected
    else if (dump.nav.speed.z > SPEED_ZERO)
    {
        return State::ASCENT;
    }
    // If the pression is too low in the igniter or combustion chamber we go to the ARMED state
    else if (dump.prop.igniter_pressure < IGNITER_PRESSURE_WANTED || dump.prop.chamber_pressure < CHAMBER_PRESSURE_WANTED)
    {
        return State::ARMED
    }
    return State::THRUSTSEQUENCE;
}


// State AvState::fromManual(DataDump dump)
// {
//     if (error() || dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
//     {
//         return State::ERRORGROUND;
//     }
//     //check all thresholds individually
//     //TODO: recheck if threholds are the wanted ones
//     else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ARM)
//     {
//         return State::ARMED;
//     }
//     return State::MANUAL;
// }

// TODO: check whether this function is more accurate than previous one
State AvState::fromManual(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ARM)
    {
        return State::ARMED;
    }
    return State::MANUAL;
}

// State AvState::fromArmed(DataDump dump)
// {
//     if (error())
//     {
//         return State::ERRORGROUND;
//     }
//     else
//     {
//         switch (dump.telemetry_cmd.id)
//         {
//             case CMD_ID::AV_CMD_IGNITION:
//                 if (dump.prop.fuel_inj_pressure >= IGNITER_PRESSURE_WANTED)
//                 {
//                     //possible log
//                     if( dump.prop.chamber_pressure >= CHAMBER_PRESSURE_WANTED ) 
//                     {
//                         //possible log
//                         return State::THRUSTSEQUENCE;
//                     }
//                     return State::ARMED;
//                 } 
//                 else 
//                 {
//                     return State::ARMED;
//                 }
//                 break;
//             case CMD_ID::AV_CMD_ABORT:
//                 return State::ERRORGROUND;
//             default:
//                 return State::ARMED;
//         }
//     }
// }

// TODO: check whether this function is more accurate than previous one
State AvState::fromArmed(DataDump dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_IGNITION)
    {
        return State::THRUSTSEQUENCE;
    }
    // TODO: add safety checks (valves open, vents open, no pressure)
    // TODO: check whether the call to the error() function should be removed
    else if (error()) 
    {
        return State::ERRORGROUND
    }
    return State::ARMED;           
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
