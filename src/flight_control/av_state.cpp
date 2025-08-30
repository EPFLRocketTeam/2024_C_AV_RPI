#include <Protocol.h>
#include "PacketDefinition_Firehorn.h"
#include "av_state.h"
#include "data.h"
#include "logger.h"
#include <iostream>


AvState::AvState()
{
    this->currentState = State::INIT;
}

// Destructor
AvState::~AvState()
{
    // Nothing to do
}

// This function allows to get the current state of the FSM
State AvState::getCurrentState()
{
    return currentState;
}

State AvState::from_init(DataDump const &dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_CALIBRATE)
    {
        Logger::log_eventf("FSM transition INIT->CALIBRATION");
        return State::CALIBRATION;
    }
    return currentState;
}

State AvState::from_calibration(DataDump const &dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition CALIBRATION->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    // If all the sensors are calibrated and ready for use we go to the FILLING state
    else if (dump.event.calibrated)
    {
        Logger::log_eventf("FSM transition CALIBRATION->FILLING");
        return State::FILLING;
    }
    return currentState;
}

State AvState::from_filling(DataDump const &dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT) 
    {
        Logger::log_eventf("FSM transition FILLING->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ARM)
    {
        Logger::log_eventf("FSM transition FILLING->ARMED");
        return State::ARMED;
    }
    return currentState;
}

State AvState::from_armed(DataDump const &dump)
{
    // Switch to fault state ABORT_ON_GROUND when receiving ABORT from ground operators
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf(Logger::WARN, "ABORT command received. FSM transition ARMED->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }

    // Automatic check of the tank pressurization
    else if (dump.event.dpr_eth_pressure_ok && dump.event.dpr_lox_pressure_ok)
    {
        Logger::log_eventf("FSM transition ARMED->PRESSURIZED");
        return State::PRESSURIZED;
    }

    // Transition to PRESSURIZED can be bypassed by ground operators
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_BYPASS_DPR_CHECK) {
        Logger::log_eventf(Logger::WARN, "Bypassing automatic pressurization check. FSM transition ARMED->PRESSURIZED");
        return State::PRESSURIZED;
    }

    return currentState;
}

State AvState::from_pressurized(DataDump const &dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT) {
        Logger::log_eventf("FSM transition PRESSURIZED->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }

    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_IGNITION)
    {
        Logger::log_eventf("FSM transition PRESSURIZED->IGNITION");
        return State::IGNITION;
    }
    return currentState;
}

State AvState::from_ignition(DataDump const &dump)
{
    if (dump.telemetry_cmd.id== CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition IGNITION->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    // TODO: condition is ignition_failed OR non_liftoff_detected
    else if (dump.event.ignition_failed)
    {
        Logger::log_eventf("FSM transition IGNITION->FILLING");
        return State::FILLING;
    }
    // If the engine is properly ignited and a liftoff has been detected we go to BURN state
    else if (dump.nav.accel.z > ACCEL_ZERO && dump.nav.altitude > ALTITUDE_ZERO && dump.event.ignited)
    {
        Logger::log_eventf("FSM transition IGNITION->BURN");
        //TODO: for VSFT must be abort
        return State::BURN;
    }
    return currentState;
}

State AvState::from_burn(DataDump const &dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition LOFTOFF->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    // If ECO is confirmed or the altitude threashold is cleared we go to the ASCENT state
    else if (dump.event.engine_cut_off || (dump.nav.altitude > ALTITUDE_THRESHOLD && dump.nav.speed.z >= SPEED_MIN_ASCENT))
    {
        Logger::log_eventf("FSM transition BURN->ASCENT");
        return State::ASCENT;
    }
    return currentState;
}

State AvState::from_ascent(DataDump const &dump)
{
    if (dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition ASCENT->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    else if (dump.nav.speed.z < SPEED_ZERO)
    {
        Logger::log_eventf("FSM transition ASCENT->DESCENT");
        return State::DESCENT;
    }
    return currentState;
}

State AvState::from_descent(DataDump const &dump)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition DESCENT->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    else if (dump.nav.speed.norm() <= SPEED_ZERO)
    {
        Logger::log_eventf("FSM transition DESCENT->LANDED");
        return State::LANDED;
    }
    return currentState;
}

State AvState::from_landed(DataDump const &dump) {
    return currentState;
}

State AvState::from_abort_ground(DataDump const &dump)
{
    if (dump.telemetry_cmd.id ==  CMD_ID::AV_CMD_RECOVER)
    {
        Logger::log_eventf("FSM transition ABORT_ON_GROUND->INIT");
        return State::INIT;
    }

    return  currentState;
}

State AvState::from_abort_flight(DataDump const &dump)
{
    return currentState;
}

void AvState::update(const DataDump &dump)
{
    switch (currentState)
    {
        case State::INIT:
            currentState = from_init(dump);
            break;
        case State::CALIBRATION:
            currentState = from_calibration(dump);
            break;
        case State::FILLING:
            currentState = from_filling(dump);
            break;
        case State::ARMED:
            currentState = from_armed(dump);
            break;
        case State::PRESSURIZED:
            currentState = from_pressurized(dump);
            break;
        case State::ABORT_ON_GROUND:
            currentState = from_abort_ground(dump);
            break;
        case State::IGNITION:
            currentState = from_ignition(dump);
            break;
        case State::BURN:
            currentState = from_burn(dump);
            break;
        case State::ASCENT:
            currentState = from_ascent(dump);
            break;
        case State::DESCENT:
            currentState = from_descent(dump);
            break;
        case State::LANDED:
            currentState = from_landed(dump);
            break;
        case State::ABORT_IN_FLIGHT:
            currentState = from_abort_flight(dump);
            break;
        default:
            currentState = State::ABORT_ON_GROUND;
    }
}
std::string AvState::stateToString(State state)
{
    switch (state)
    {
        case State::INIT:
            return "INIT";
            break;
        case State::CALIBRATION:
            return "CALIBRATION";
            break;
        case State::FILLING:
            return "FILLING";
            break;
        case State::ARMED:
            return "ARMED";
            break;
        case State::PRESSURIZED:
            return "PRESSURIZED";
            break;
        case State::ABORT_ON_GROUND:
            return "ABORT_ON_GROUND";
            break;
        case State::IGNITION:
            return "IGNITION";
            break;
        case State::BURN:
            return "BURN";
            break;
        case State::ASCENT:
            return "ASCENT";
            break;
        case State::DESCENT:
            return "DESCENT";
            break;
        case State::LANDED:
            return "LANDED";
            break;
        case State::ABORT_IN_FLIGHT:
            return "ABORT_IN_FLIGHT";
            break;
        default:
            return "N/A";
            break;
    }
}



