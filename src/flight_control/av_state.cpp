#include <Protocol.h>
#include "PacketDefinition_Firehorn.h"
#include "av_state.h"
#include "data.h"
#include "logger.h"
#include <iostream>

MovingAverage::MovingAverage(size_t power) : maxSize(power), sum(0.0f)
{
    samples.reserve(1 << power);
}

void MovingAverage::addSample(float sample)
{
    if (samples.size() >= (1 << maxSize))
    {
        sum -= samples.front();
        samples.erase(samples.begin());
    }

    samples.push_back(sample);
    sum += sample;
}

float MovingAverage::getAverage() const
{
    if (samples.empty())
    {
        return 0.0f; // or handle empty case as needed
    }
    // Could opti if we remove division by having a fix size TBD
    return sum / samples.size();
}

void MovingAverage::reset()
{
    samples.clear();
    sum = 0.0f;
}
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

State AvState::from_init(DataDump const &dump, uint32_t delta_ms)

{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_CALIBRATE)
    {
        Logger::log_eventf("FSM transition INIT->CALIBRATION");
        return State::CALIBRATION;
    }
    return currentState;
}

State AvState::from_calibration(DataDump const &dump, uint32_t delta_ms)
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

State AvState::from_filling(DataDump const &dump, uint32_t delta_ms)
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

State AvState::from_armed(DataDump const &dump, uint32_t delta_ms)
{
    // Switch to fault state ABORT_ON_GROUND when receiving ABORT from ground operators
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf(Logger::WARN, "ABORT command received. FSM transition ARMED->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }

    else if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_PRESSURIZE)
    {
        Logger::log_eventf("FSM transition ARMED->PRESSURIZED");
        return State::PRESSURIZATION;
    }

    // Transition to PRESSURIZED can be bypassed by ground operators
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_BYPASS_DPR_CHECK)
    {
        Logger::log_eventf(Logger::WARN, "Bypassing automatic pressurization check. FSM transition ARMED->PRESSURIZED");
        return State::IGNITION;
    }

    return currentState;
}

void inline reset_pressurization(MovingAverage &avg_lox, MovingAverage &avg_fuel, uint32_t &start_time)
{
    avg_lox.reset();
    avg_fuel.reset();
    start_time = 0;
}

State AvState::from_pressurization(DataDump const &dump, uint32_t delta_ms)
{
    switch (dump.telemetry_cmd.id)
    {
    case CMD_ID::AV_CMD_ABORT:
    {
        Logger::log_eventf("FSM transition PRESSURIZED->ABORT_ON_GROUND");
        reset_pressurization(pressure_lox_avg, pressure_fuel_avg, pressurization_start_time);
        return State::ABORT_ON_GROUND;
    }

    case CMD_ID::AV_CMD_IGNITION:
    {
        reset_pressurization(pressure_lox_avg, pressure_fuel_avg, pressurization_start_time);
        Logger::log_eventf("FSM transition PRESSURIZED->IGNITION");
        return State::IGNITION;
    }

    default:
    {
        // advance timer
        pressurization_start_time += delta_ms;

        // update moving averages
        pressure_lox_avg.addSample(dump.prop.LOX_pressure);
        pressure_fuel_avg.addSample(dump.prop.fuel_pressure);

        // cache averages (avoid repeated calls)
        const auto fuel_avg = pressure_fuel_avg.getAverage();
        const auto lox_avg = pressure_lox_avg.getAverage();

        // allow a short grace period after entering this state before triggering overpressure aborts

        if (pressurization_start_time > OVERPRESSURE_GRACE_MS)
        {
            if (fuel_avg > FUEL_PRESSURE_MAX || lox_avg > LOX_PRESSURE_MAX)
            {
                reset_pressurization(pressure_lox_avg, pressure_fuel_avg, pressurization_start_time);
                Logger::log_eventf(
                    Logger::ERROR,
                    "Tank overpressure detected! Fuel: %.2f / LOX: %.2f. "
                    "FSM transition PRESSURIZED->ABORT_ON_GROUND",
                    fuel_avg, lox_avg);
                return State::ABORT_ON_GROUND;
            }
        }

        // success path once we've waited long enough
        if (pressurization_start_time > PRESSURIZATION_WAIT_TIME &&
            fuel_avg >= FUEL_PRESSURE_WANTED &&
            lox_avg >= LOX_PRESSURE_WANTED)
        {
            reset_pressurization(pressure_lox_avg, pressure_fuel_avg, pressurization_start_time);
            Logger::log_eventf("Pressurization successful");
            return State::IGNITION;
        }

        return currentState;
    }
    }
}

void inline reset_ignition_timers(uint32_t &timer_accel, uint32_t &timer_liftoff_timeout)
{
    timer_accel = 0;
    timer_liftoff_timeout = 0;
}

State AvState::from_ignition(DataDump const &dump, uint32_t delta_ms)
{
    timer_liftoff_timeout += delta_ms;
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition IGNITION->ABORT_ON_GROUND");
        reset_ignition_timers(timer_accel, timer_liftoff_timeout);
        return State::ABORT_ON_GROUND;
    }

    if (dump.nav.accel.z > ACCEL_LIFTOFF)
    {
        timer_accel += delta_ms;
    }
    else
    {
        timer_accel = 0;
    }
    if (timer_accel > 500)
    {
        if (dump.nav.altitude > ALTITUDE_ZERO)
        {
            reset_ignition_timers(timer_accel, timer_liftoff_timeout);
            Logger::log_eventf("Liftoff detected");
            return State::BURN;
        }
        else
        {
            reset_ignition_timers(timer_accel, timer_liftoff_timeout);
            Logger::log_eventf("Liftoff detected but altitude too low, aborting");
            return State::ABORT_IN_FLIGHT;
        }
    }

    // TODO: condition is ignition_failed OR non_liftoff_detected
    else if (dump.event.ignition_failed || timer_liftoff_timeout > MAX_LIFTOFF_TIMEOUT)
    {
        reset_ignition_timers(timer_accel, timer_liftoff_timeout);
        Logger::log_eventf("FSM transition IGNITION->FILLING");
        return State::FILLING;
    }

    return currentState;
}

State AvState::from_burn(DataDump const &dump, uint32_t delta_ms)
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

State AvState::from_ascent(DataDump const &dump,uint32_t delta_ms)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition ASCENT->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    //TODO: better apogee detection
    else if (dump.nav.speed.z < SPEED_ZERO)
    {
        Logger::log_eventf("FSM transition ASCENT->DESCENT");
        return State::DESCENT;
    }
    return currentState;
}

State AvState::from_descent(DataDump const &dump, uint32_t delta_ms)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition DESCENT->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    //TODO:probably a safety against an exactly zero speed apogee detection like a timer of 200ms
    else if (dump.nav.speed.norm() <= SPEED_ZERO)
    {
        Logger::log_eventf("FSM transition DESCENT->LANDED");
        return State::LANDED;
    }
    return currentState;
}

State AvState::from_landed(DataDump const &dump, uint32_t delta_ms)
{
    return currentState;
}

State AvState::from_abort_ground(DataDump const &dump, uint32_t delta_ms)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_RECOVER)
    {
        Logger::log_eventf("FSM transition ABORT_ON_GROUND->INIT");
        return State::INIT;
    }

    return currentState;
}

State AvState::from_abort_flight(DataDump const &dump, uint32_t delta_ms)
{
    return currentState;
}

void AvState::update(const DataDump &dump, uint32_t delta_ms)
{
    switch (currentState)
    {
    case State::INIT:
        currentState = from_init(dump, delta_ms);
        break;
    case State::CALIBRATION:
        currentState = from_calibration(dump, delta_ms);
        break;
    case State::FILLING:
        currentState = from_filling(dump, delta_ms);
        break;
    case State::ARMED:
        currentState = from_armed(dump, delta_ms);
        break;
    case State::PRESSURIZATION:
        currentState = from_pressurization(dump, delta_ms);
        break;
    case State::ABORT_ON_GROUND:
        currentState = from_abort_ground(dump, delta_ms);
        break;
    case State::IGNITION:
        currentState = from_ignition(dump, delta_ms);
        break;
    case State::BURN:
        currentState = from_burn(dump, delta_ms);
        break;
    case State::ASCENT:
        currentState = from_ascent(dump, delta_ms);
        break;
    case State::DESCENT:
        currentState = from_descent(dump, delta_ms);
        break;
    case State::LANDED:
        currentState = from_landed(dump, delta_ms);
        break;
    case State::ABORT_IN_FLIGHT:
        currentState = from_abort_flight(dump, delta_ms);
        break;
    default:
        currentState = State::ABORT_ON_GROUND;
    }
    Data::get_instance().write(Data::AV_STATE, &currentState);
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
    case State::PRESSURIZATION:
        return "PRESSURIZATION";
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
