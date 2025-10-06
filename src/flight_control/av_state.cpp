#include <Protocol.h>
#include "PacketDefinition_Firehorn.h"
#include "av_state.h"
#include "data.h"
#include "logger.h"
#include "av_timer.h"
#include <iostream>



///////////////////////////////
// AvState: Flight Computer FSM
///////////////////////////////
AvState::AvState()
:   pressurization_start_time(0),
    timer_accel(0),
    timer_liftoff_timeout(0)

{
    reset_flight();
}

// Destructor
AvState::~AvState()
{
    // Nothing to do
}

void AvState::reset_flight() {
    pressure_fuel_avg.reset();
    pressure_lox_avg.reset();
    pressurization_start_time = 0;
    timer_accel = 0;
    timer_liftoff_timeout = 0;
    ascent_elapsed = 0;
    descent_elapsed = 0;
    accel_g_offset = 0;
    this->currentState = State::INIT;
    Data::get_instance().reset_events();
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
        const int samples(1000);
        float sum(0);
        for (int i(0); i < samples; ++i) {
            float acc_x(dump.nav.accel.x);
            sum += acc_x;
            AvTimer::sleep(1);
        }
        accel_g_offset = sum / samples;
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
        Logger::log_eventf("FSM transition ARMED->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_PRESSURIZE)
    {
        Logger::log_eventf("FSM transition ARMED->PRESSURIZATION");
        Logger::log_eventf(Logger::WARN, "PRESSURIZATION is imminent");
        return State::PRESSURIZATION;
    }

    return currentState;
}

//TODO: should be in h_driver
void inline reset_pressurization(MovingAverage &avg_lox, MovingAverage &avg_fuel, uint32_t &start_time)
{
    avg_lox.reset();
    avg_fuel.reset();
    start_time = 0;
}

State AvState::from_pressurization(DataDump const &dump, uint32_t delta_ms)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition PRESSURIZED->ABORT_ON_GROUND");
        reset_pressurization(pressure_lox_avg, pressure_fuel_avg, pressurization_start_time);
        return State::ABORT_ON_GROUND;
    }

    // advance timer
    pressurization_start_time += delta_ms;

    // update moving averages
    if (-10 <= dump.prop.LOX_pressure && dump.prop.LOX_pressure <= 1000) {
        pressure_lox_avg.addSample(dump.prop.LOX_pressure);
    }
    if (-10 <= dump.prop.fuel_pressure && dump.prop.fuel_pressure <= 1000) {
        pressure_fuel_avg.addSample(dump.prop.fuel_pressure);
    }

    const float fuel_avg = pressure_fuel_avg.getAverage();
    const float lox_avg = pressure_lox_avg.getAverage();

    // allow a short grace period after entering this state before triggering overpressure aborts
    if (pressurization_start_time > OVERPRESSURE_GRACE_MS)
    {
        if (fuel_avg > PRESSURIZATION_CHECK_PRESSURE || lox_avg > PRESSURIZATION_CHECK_PRESSURE)
        {
            reset_pressurization(pressure_lox_avg, pressure_fuel_avg, pressurization_start_time);
            Logger::log_eventf(
                    Logger::FATAL,
                    "Tank overpressure detected! Fuel: %.2f / LOX: %.2f. ",
                    fuel_avg, lox_avg);
            Logger::log_eventf("FSM transition PRESSURIZED->ABORT_ON_GROUND");
            return State::ABORT_ON_GROUND;
        }
    }

    // success path once we've waited long enough
    Logger::log_eventf("pressurization_elapsed: %u", pressurization_start_time);
    Logger::log_eventf("Fuel Avg: %f | LOx avg: %f", fuel_avg, lox_avg);
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_LAUNCH )
    {
        if (fuel_avg <= PRESSURIZATION_CHECK_PRESSURE &&
            lox_avg <= PRESSURIZATION_CHECK_PRESSURE) {
            reset_pressurization(pressure_lox_avg, pressure_fuel_avg, pressurization_start_time);
            Logger::log_eventf("Pressurization successful");
            Logger::log_eventf("FSM transition PRESSURIZATION->IGNITION");
            Logger::log_eventf(Logger::WARN, "IGNITION is imminent");
            return State::IGNITION;
        }
    }

    return currentState;
}

void inline reset_ignition_timers(uint32_t &timer_accel, uint32_t &timer_liftoff_timeout, uint32_t &timer_burn_timeout)
{
    timer_accel = 0;
    timer_liftoff_timeout = 0;
    timer_burn_timeout = 0;
}

State AvState::from_ignition(DataDump const &dump, uint32_t delta_ms)
{
    timer_liftoff_timeout += delta_ms;
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
        Logger::log_eventf("FSM transition IGNITION->ABORT_ON_GROUND");
        reset_ignition_timers(timer_accel, timer_liftoff_timeout, timer_burn_timeout);
        return State::ABORT_ON_GROUND;
    }

    
    if (0/*dump.nav.accel.x > ACCEL_LIFTOFF*/)
    {
        timer_accel += delta_ms;
    }
    else
    {
        timer_accel = 0;
    }
    if (timer_accel > ACCEL_LIFTOFF_DURATION_MS)
    {
        Logger::log_eventf(Logger::WARN, "Vertical acceleration detected. ABORTING");
        Logger::log_eventf("FSM transition IGNITION->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
    }
    

    if (dump.event.ignited) {
        Logger::log_eventf("FSM transition IGNITION->BURN");
        return State::BURN;
    }

    else if (dump.event.ignition_failed)
    {
        reset_ignition_timers(timer_accel, timer_liftoff_timeout, timer_burn_timeout);
        Logger::log_eventf(Logger::WARN, "BURN SEQUENCE ABORTION");
#if (ABORT_FLIGHT_EN)
        Logger::log_eventf("FSM transition IGNITION->ABORT_IN_FLIGHT");
        return State::ABORT_IN_FLIGHT;
#else
        Logger::log_eventf("FSM transition IGNITION->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
#endif
    }

    return currentState;
}

State AvState::from_burn(DataDump const &dump, uint32_t delta_ms)
{
    timer_burn_timeout += delta_ms;
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
#if (ABORT_FLIGHT_EN)
        Logger::log_eventf("FSM transition BURN->ABORT_IN_FLIGHT");
        return State::ABORT_IN_FLIGHT;
#else
        Logger::log_eventf("FSM transition BURN->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
#endif
    }
    // If ECO is confirmed we go to the ASCENT state
    // New condition, after a certain burn time, we force the transition
    if (dump.event.engine_cut_off || timer_burn_timeout > BURN_MAX_DURATION_MS)
    {
        Logger::log_eventf("FSM transition BURN->ASCENT");
        return State::ASCENT;
    }
    
    return currentState;
}

State AvState::from_ascent(DataDump const &dump,uint32_t delta_ms)
{
    Logger::log_eventf("ASCENT elapsed: %u", ascent_elapsed);
    if(dump.nav.speed < 0.0){
        
        apogee_counter++;
        Logger::log_eventf("Negative speed detected = %f for the %d consecutive time", dump.nav.speed,apogee_counter);
    }
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
#if (ABORT_FLIGHT_EN)
        Logger::log_eventf("FSM transition BURN->ABORT_IN_FLIGHT");
        return State::ABORT_IN_FLIGHT;
#else
        Logger::log_eventf("FSM transition BURN->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
#endif
    }
    //TODO: better apogee detection
    else if (apogee_counter>=4 || ascent_elapsed > ASCENT_MAX_DURATION_MS)
    {
        Logger::log_eventf("FSM transition ASCENT->DESCENT");
        return State::DESCENT;
    }
    ascent_elapsed += delta_ms;
    return currentState;
}

State AvState::from_descent(DataDump const &dump, uint32_t delta_ms)
{
    if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT)
    {
#if (ABORT_FLIGHT_EN)
        Logger::log_eventf("FSM transition BURN->ABORT_IN_FLIGHT");
        return State::ABORT_IN_FLIGHT;
#else
        Logger::log_eventf("FSM transition BURN->ABORT_ON_GROUND");
        return State::ABORT_ON_GROUND;
#endif
    }
    //TODO:probably a safety against an exactly zero speed apogee detection like a timer of 200ms
    else if (dump.nav.speed.norm() <= SPEED_ZERO && descent_elapsed > 180e3)
    {
        Logger::log_eventf("FSM transition DESCENT->LANDED");
        return State::LANDED;
    }

    Logger::log_eventf("DESCENT elapsed: %u", descent_elapsed);
    descent_elapsed += delta_ms;
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
        Logger::log_eventf(Logger::WARN, "RECOVER command received, resetting flight variables");
        reset_flight();
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
