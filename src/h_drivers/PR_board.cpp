#include <iostream>
#include <string>
#include "PR_board.h"
#include "data.h"
#include "av_state.h"
#include "i2c_interface.h"
#include "logger.h"
#include "intranet_commands.h"
#include "config.h"
#include "thresholds.h"

// Enumeration for the command of the PR board

// FIXME: implement the valve logic later on
PR_board::PR_board() {
    try {
        I2CInterface::getInstance().open(AV_NET_ADDR_PRB);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf(Logger::FATAL, "Error during PRB I2C initilazation: %s", e.what());
    }    
    reset_counters(); 
}

PR_board::~PR_board() {
    try {
        I2CInterface::getInstance().close(AV_NET_ADDR_PRB);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf(Logger::ERROR, "Error during PRB I2C deinitialization: %s", e.what());
    }
}

void PR_board::write_timestamp() {
    const uint32_t timestamp(Data::get_instance().get().av_timestamp);
    write_register(AV_NET_PRB_TIMESTAMP, (uint8_t*)&timestamp);
    Logger::log_eventf(Logger::DEBUG, "Writing TIMESTAMP to PRB: %u", timestamp);
}

void PR_board::send_wake_up() {
    const uint32_t order(AV_NET_CMD_ON);
    write_register(AV_NET_PRB_WAKE_UP, (uint8_t*)&order);
}

void PR_board::send_sleep() {
    const uint32_t order(AV_NET_CMD_OFF);
    write_register(AV_NET_PRB_WAKE_UP, (uint8_t*)&order);
}

void PR_board::send_reset() {
    const uint32_t order(AV_NET_CMD_ON);
    write_register(AV_NET_PRB_RESET, (uint8_t*)&order);
    Logger::log_eventf(Logger::DEBUG, "Sending RESET to PRB");
}

bool PR_board::read_is_woken_up() {
    uint32_t rslt(0);
    read_register(AV_NET_PRB_IS_WOKEN_UP, (uint8_t*)&rslt);

    bool prb_woken_up(rslt == AV_NET_CMD_ON);
    Data::get_instance().write(Data::EVENT_PRB_READY, &prb_woken_up);
    return prb_woken_up;
}

void PR_board::clear_to_ignite(bool value) {
    uint32_t cmd(value ? AV_NET_CMD_ON : AV_NET_CMD_OFF);
    write_register(AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&cmd);
    Logger::log_eventf(Logger::DEBUG, "Writing CLEAR_TO_IGNITE to PRB");
}

void PR_board::send_passivate() {
    uint32_t cmd(AV_NET_CMD_ON);
    write_register(AV_NET_PRB_PASSIVATE, (uint8_t*)&cmd);
    Logger::log_eventf(Logger::DEBUG, "Sending PASSIVATE to PRB");
}

void PR_board::read_fsm() {
    uint32_t state(0);
    read_register(AV_NET_PRB_FSM_PRB, (uint8_t*)&state);
    Data::get_instance().write(Data::PR_BOARD_FSM_STATE, &state);
    Logger::log_eventf(Logger::DEBUG, "Reading FSM from PRB: %u", state);
}

void PR_board::read_injector_oxygen() {
    float pressure(0);
    float temperature(0);

    read_register(AV_NET_PRB_P_OIN, (uint8_t*)&pressure);
    read_register(AV_NET_PRB_T_OIN, (uint8_t*)&temperature);

    if (std::isfinite(pressure) && CHECK_TANK_PRESS(pressure)) {
        Data::get_instance().write(Data::PR_SENSOR_P_OIN, &pressure);
    }
    if (std::isfinite(temperature) && CHECK_TEMPERATURE(temperature)) {
        Data::get_instance().write(Data::PR_SENSOR_T_OIN, &temperature);
    }

    Logger::log_eventf(Logger::DEBUG, "Reading P_OIN from PRB: %f", pressure);
    Logger::log_eventf(Logger::DEBUG, "Reading T_OIN from PRB: %f", temperature);
}

void PR_board::read_injector_fuel() {
    float pressure(0);
    float temperature(0);

    read_register(AV_NET_PRB_P_EIN, (uint8_t*)&pressure);
    read_register(AV_NET_PRB_T_EIN, (uint8_t*)&temperature);

    if (std::isfinite(pressure) && CHECK_TANK_PRESS(pressure)) {
        Data::get_instance().write(Data::PR_SENSOR_P_EIN, &pressure);
    }
    if (std::isfinite(temperature) && CHECK_TEMPERATURE(temperature)) {
        Data::get_instance().write(Data::PR_SENSOR_T_EIN, &temperature);
    }

    Logger::log_eventf(Logger::DEBUG, "Reading P_EIN from PRB: %f", pressure);
    Logger::log_eventf(Logger::DEBUG, "Reading T_EIN from PRB: %f", temperature);
}

void PR_board::read_injector_cooling_temperature() {
    float temperature(0);
    read_register(AV_NET_PRB_T_EIN_PT1000, (uint8_t*)&temperature);
    if (std::isfinite(temperature) && CHECK_TEMPERATURE(temperature)) {
        Data::get_instance().write(Data::PR_SENSOR_T_EIN_CF, &temperature);
    }
    Logger::log_eventf(Logger::DEBUG, "Reading T_EIN_PT1000 from PRB: %f", temperature);
}

void PR_board::read_combustion_chamber() {
    float pressure(0);
    float temperature(0);

    read_register(AV_NET_PRB_P_CCC, (uint8_t*)&pressure);
    read_register(AV_NET_PRB_T_CCC, (uint8_t*)&temperature);

    if (std::isfinite(pressure) && CHECK_TANK_PRESS(pressure)) {
        Data::get_instance().write(Data::PR_SENSOR_P_CCC, &pressure);
    }
    if (std::isfinite(temperature) && CHECK_TEMPERATURE(temperature)) {
        Data::get_instance().write(Data::PR_SENSOR_T_CCC, &temperature);
    }

    Logger::log_eventf(Logger::DEBUG, "Reading P_CCC from PRB: %f", pressure);
    Logger::log_eventf(Logger::DEBUG, "Reading T_CCC from PRB: %f", temperature);
}

void PR_board::write_igniter(uint32_t cmd) {
    write_register(AV_NET_PRB_IGNITER, (uint8_t*)&cmd);
    Logger::log_eventf(Logger::DEBUG, "Writing IGNITER to PRB: %u", cmd);
}

void PR_board::write_valves(const uint32_t cmd) {
    write_register(AV_NET_PRB_VALVES_STATE, (uint8_t*)&cmd);
    Logger::log_eventf(Logger::DEBUG, "Writing valves to PRB: %x", cmd);
}

uint32_t PR_board::read_valves() {
    uint32_t rslt(0);
    read_register(AV_NET_PRB_VALVES_STATE, (uint8_t*)&rslt);

    uint8_t main_lox((rslt & AV_NET_PRB_VALVE_MO_BC) >> AV_NET_SHIFT_MO_BC);
    uint8_t main_fuel((rslt & AV_NET_PRB_VALVE_ME_B) >> AV_NET_SHIFT_ME_B);

    Valves valves(Data::get_instance().get().valves);

    if (main_lox == AV_NET_CMD_ON) {
        valves.valve_prb_main_lox = 1;
    }else if (main_lox == AV_NET_CMD_OFF) {
        valves.valve_prb_main_lox = 0;
    }
    
    if (main_fuel == AV_NET_CMD_ON) {
        valves.valve_prb_main_fuel = 1;
    }else if (main_fuel == AV_NET_CMD_OFF) {
        valves.valve_prb_main_fuel = 0;
    }

    Data::get_instance().write(Data::VALVES, &valves);

    Logger::log_eventf(Logger::DEBUG, "Reading valves from PRB: %x", rslt);

    return rslt;
}

float PR_board::read_pressure_check() {
    float rslt(0);
    read_register(AV_NET_PRB_PRESSURE_CHECK, (uint8_t*)&rslt);
    Data::get_instance().write(Data::PR_PRESSURE_CHECK, &rslt);
    Logger::log_eventf(Logger::DEBUG, "Reading pressure check from PRB: %f", rslt);

    return rslt;
}

float PR_board::read_impulse(const DataDump& dump) {
    float rslt(0);
    read_register(AV_NET_PRB_SPECIFIC_IMP, (uint8_t*)&rslt);
    Data::get_instance().write(Data::PR_TOTAL_IMPULSE, &rslt);
    Logger::Severity severity(dump.av_state == State::IGNITION || dump.av_state == State::BURN ?
            Logger::INFO : Logger::DEBUG);
    Logger::log_eventf(Logger::DEBUG, "Reading specific impulse from PRB: %f", rslt);

    return rslt;
}


// TODO: Review check policy FLIGHT LOGIC
void PR_board::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    this->delta_ms = delta_ms;
    switch (dump.av_state) {
        case State::INIT:
            handle_init(dump);
            break;
        case State::CALIBRATION:
            // Handle calibration logic
            handle_calibration(dump);
            break;
        case State::FILLING:
            handle_filling(dump);
            break;
        case State::ARMED:
            handle_armed(dump);
            break;
        case State::PRESSURIZATION:
            handle_pressurization(dump);
            break;
        case State::IGNITION:
            handle_ignition(dump);
            break;
        case State::BURN:
            handle_burn(dump);
            break;
        case State::ASCENT:
            handle_ascent(dump);
            break;
        case State::DESCENT:
            handle_descent(dump);
            break;
        case State::ABORT_ON_GROUND:
            handle_abort_ground(dump);
            break;
        case State::ABORT_IN_FLIGHT:
            handle_abort_flight(dump);
            break;
        default:
            break;
    }

    read_fsm();
    read_valves();
    read_injector_oxygen();
    read_injector_fuel();
    read_injector_cooling_temperature();
    read_combustion_chamber();
    read_impulse(dump);
}

void PR_board::actuate_valve(const bool active, const uint8_t valve_bitshift) {
    uint32_t cmd(0);
    if (active == 1) {
        cmd = AV_NET_CMD_ON << valve_bitshift;
    }else if (active == 0) {
        cmd = AV_NET_CMD_OFF << valve_bitshift;
    }
    uint32_t valves(read_valves());
    valves &= ~(0xFF << valve_bitshift);
    valves |= cmd;
    write_valves(valves);
}

void PR_board::reset_counters() {
    count_ms = 0;
    polling_count_ms = 0;
    ignition_send_ms = 0;
    ignition_ack_ms = 0;
    ignition_sq_started = 0;
    ignited = 1;
    burn_elapsed_ms = 0;
    passivation_count_ms = 0;
}

void PR_board::handle_init(const DataDump& dump) {
    reset_counters();
    uint32_t default_valves(AV_NET_CMD_OFF << AV_NET_SHIFT_MO_BC
            | AV_NET_CMD_OFF << AV_NET_SHIFT_ME_B);
    write_valves(default_valves);
    send_reset();
}

void PR_board::handle_calibration(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);

    // PRB will be in idle mode (ready)
    if (!dump.event.prb_ready) {
        bool prb_ready(true);
        Data::get_instance().write(Data::EVENT_PRB_READY, &prb_ready);
    }
}

void PR_board::handle_filling(const DataDump& dump) {
    // Process commands in manual mode
    periodic_timestamp(100);
    listen_valves_command(dump);
}

void PR_board::handle_armed(const DataDump& dump) {
    periodic_timestamp(100);
    listen_valves_command(dump);
}

void PR_board::handle_pressurization(const DataDump& dump) {
    // Write timestamp + clear to trigger at 10Hz
    periodic_timestamp(100);

    listen_valves_command(dump);
    read_pressure_check();

    if (dump.event.ignition_failed) {
        bool trigger_failed = false;
        Data::get_instance().write(Data::EVENT_IGNITION_FAILED, &trigger_failed);
    }

    if (dump.prop.PRB_state != PRB_FSM::CLEAR_TO_IGNITE) {
        clear_to_ignite(1);
    }
}

void PR_board::handle_ignition(const DataDump& dump) {
    const PRB_FSM prb_state((PRB_FSM)dump.prop.PRB_state);
    const Valves valves(dump.valves);
    if (!dump.event.ignited && !dump.event.ignition_failed) {
        // While PRB hasn't switched to IGNITION_SQ, send IGNITER cmd
        if (prb_state != PRB_FSM::IGNITION_SQ) {
            // Send IGNITER command to PRB to start the ignition sequence
            write_igniter(AV_NET_CMD_ON);

            if (ignition_send_ms > IGNITION_NO_COM_TIMEOUT_MS) {
                Logger::log_eventf(Logger::FATAL, "Prop Board failed to enter IGNITION_SQ after %u ms",
                        IGNITION_NO_COM_TIMEOUT_MS);
                ignition_send_ms = 0;
            }
            Logger::log_eventf(Logger::DEBUG, "ignition com elapsed: %u", ignition_send_ms);
            ignition_send_ms += delta_ms;
        }else if (!ignition_sq_started) {
            ignition_sq_started = true;
            Logger::log_eventf("IGNITION SEQUENCE STARTED");
        }

        // After some delay, check the PRB FSM for ignition status
        const bool engine_valves_open(valves.valve_prb_main_lox && valves.valve_prb_main_fuel);
        if (ignition_sq_started && ignition_ack_ms > IGNITION_ACK_DELAY_MS && engine_valves_open) {
            if (prb_state == PRB_FSM::ABORT) {
                ignited = false;
            }
            Data::get_instance().write(Data::EVENT_IGNITED, &ignited);
            bool ignition_failure(!ignited);
            Data::get_instance().write(Data::EVENT_IGNITION_FAILED, &ignition_failure);

            if (ignited) {
                Logger::log_eventf("IGNITION CONFIRMED. CHAMBER PRESSURE NOMINAL: %f [bar]", dump.prop.pressure_check);
            }else if (ignition_failure) {
                Logger::log_eventf("IGNITION FAILED. CHAMBER PRESSURE TOO LOW: %f [bar]", dump.prop.pressure_check);
            }
        }
        Logger::log_eventf(Logger::DEBUG, "ignition check elapsed: %u", ignition_ack_ms);
        ignition_ack_ms += delta_ms;
    }
    read_pressure_check();
}

void PR_board::handle_burn(const DataDump& dump) {
    periodic_timestamp(100);
    bool cut_off(true);
    if (!dump.valves.valve_prb_main_lox) {
        Data::get_instance().write(Data::EVENT_ENGINE_CUT_OFF, &cut_off);
    }
    burn_elapsed_ms += delta_ms;
    Logger::log_eventf("BURN elapsed: %u", burn_elapsed_ms);
}

void PR_board::handle_ascent(const DataDump& dump) {
    periodic_timestamp(100);
}

void PR_board::handle_descent(const DataDump& dump) {
    periodic_timestamp(100);
    if (passivation_count_ms >= PASSIVATION_DELAY_AFTER_APOGEE) {
       send_passivate();
    }else {
        passivation_count_ms += delta_ms;
    }
}

void PR_board::handle_abort_ground(const DataDump& dump) {
    periodic_timestamp(1e3);
    uint8_t passivate(AV_NET_CMD_OFF);
    write_register(AV_NET_PRB_ABORT, &passivate);
    Logger::log_eventf(Logger::DEBUG, "Sending ABORT to PRB. NO PASSIVATION");
}

void PR_board::handle_abort_flight(const DataDump& dump) {
    periodic_timestamp(1e2);
    uint8_t passivate(AV_NET_CMD_ON);
    write_register(AV_NET_PRB_ABORT, &passivate);
    Logger::log_eventf(Logger::DEBUG, "Sending ABORT to PRB. WITH PASSIVATION");
}


void PR_board::read_register(const uint8_t reg_addr, uint8_t* data) {
    try {
        I2CInterface::getInstance().read(AV_NET_ADDR_PRB, reg_addr, data, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("PRB communication error: failed reading from register ");
        throw PRBoardException(msg + std::to_string(reg_addr) + "\t(" + e.what() + ")");
    }
}

void PR_board::write_register(const uint8_t reg_addr, const uint8_t* data) {
 
    try {
        I2CInterface::getInstance().write(AV_NET_ADDR_PRB, reg_addr, data, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("PRB communication error: failed writing to register ");
        throw PRBoardException(msg + std::to_string(reg_addr) + "\t(" + e.what() + ")");
    }
}

void PR_board::listen_valves_command(const DataDump& dump) {
    if (dump.event.command_updated) {
        const uint8_t value(dump.telemetry_cmd.value);
        switch (dump.telemetry_cmd.id) {
            case CMD_ID::AV_CMD_MAIN_LOX:
                // Handle main LOX valve commands
                actuate_valve(value, AV_NET_SHIFT_MO_BC);
                break;
            case CMD_ID::AV_CMD_MAIN_FUEL:
                // Handle main Fuel valve commands
                actuate_valve(value, AV_NET_SHIFT_ME_B);
                break;
            default:
                break;
        }
    }
}

inline void PR_board::periodic_timestamp(const uint32_t period) {
    count_ms += delta_ms;
    if (count_ms >= period) {
        write_timestamp();
        count_ms = 0;
    }
}

