#include "PR_board.h"
#include "data.h"
#include "av_state.h"
#include <iostream>
#include <pigpio.h>
#include <string>
#include "i2c_interface.h"
#include "logger.h"
#include "intranet_commands.h"

#define TIMEOUT_MS 1000
#define TIME_ATTEMPT_MS 100
#define HIGH_PERIOD 1000
#define LOW_PERIOD 100

// Enumeration for the command of the PR board

// FIXME: implement the valve logic later on
PR_board::PR_board() {
      try {
        I2CInterface::getInstance().open(AV_NET_ADDR_PRB);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf("Error during PRB I2C initilazation: %s", e.what());
    }    
}

PR_board::~PR_board() {
    try {
        I2CInterface::getInstance().close(AV_NET_ADDR_PRB);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf("Error during PRB I2C deinitialization: %s", e.what());
    }
}

inline void PR_board::count_time(uint32_t period,uint32_t delta_ms){
    count_ms += delta_ms;
    if (count_ms >= period) {
        write_timestamp();
        count_ms = 0;
    }
}

inline void PR_board::none_init_baseHandler(uint32_t period,uint32_t delta_ms){
    count_ms += delta_ms;
    if (count_ms >= period) {
        write_timestamp();
        count_ms = 0;
        uint8_t state(0);
        read_register(AV_NET_PRB_FSM_PRB, &state);
        //TODO probably write all
        Data::get_instance().write(Data::PR_BOARD_FSM_STATE, &state);
    }
}

// fixme: implement the valve logic later on 
// PR_board::PR_board() {
//     // Initialize all valves to closed (0 degrees)
//     for (auto& valve_state : current_valve_states) {
//         valve_state = static_cast<uint8_t>(ValveOpenDegree::DEG_0);
//     }
//     // TODO: should modify the goat accordingly 
//     // inline function
// }

// // Write a single valve's degree
// void PR_board::write_valve(uint8_t valve_id, ValveOpenDegree degree) {
//     // If the valve ID is invalid, print an error message and return
//     if (valve_id >= NUM_VALVES) {
//         std::cerr << "Invalid valve ID: " << static_cast<int>(valve_id) << "\n";
//         return;
//     }

//     current_valve_states[valve_id] = static_cast<uint8_t>(degree) & 0x0F; // Mask to 4 bits

//     // TODO: implement actual hardware control if needed
// }

// // Read a single valve's degree
// ValveOpenDegree PR_board::read_valve(uint8_t valve_id) const {
//     // If the valve ID is invalid, print an error message and return 0 degrees
//     if (valve_id >= NUM_VALVES) {
//         std::cerr << "Invalid valve ID: " << static_cast<int>(valve_id) << "\n";
//         return ValveOpenDegree::DEG_0; // Default to 0 degrees on error
//     }

//     // Mask the 4 LSB to get the actual degree
//     return static_cast<ValveOpenDegree>(current_valve_states[valve_id] & 0x0F);
// }


void PR_board::read_register(const uint8_t reg_addr, uint8_t* data) {
    try {
        I2CInterface::getInstance().read(AV_NET_ADDR_PRB, reg_addr, data, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("PRB communication error: failed reading from register ");
        throw PRBoardException(msg + std::to_string(reg_addr) + "\n\t" + e.what());
    }
}

void PR_board::write_register(const uint8_t reg_addr, const uint8_t* data) {
    try {
        I2CInterface::getInstance().write(AV_NET_ADDR_PRB, reg_addr, data, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("PRB communication error: failed writing to register ");
        throw PRBoardException(msg + std::to_string(reg_addr) + "\n\t" + e.what());
    }
}

void PR_board::write_timestamp() {
    const uint32_t timestamp(Data::get_instance().get().av_timestamp);
    write_register(AV_NET_PRB_TIMESTAMP, (uint8_t*)&timestamp);
}

void PR_board::send_wake_up() {
    const uint32_t order(AV_NET_CMD_ON);
    write_register(AV_NET_PRB_WAKE_UP, (uint8_t*)&order);
}

void PR_board::send_sleep() {
    const uint32_t order(AV_NET_CMD_OFF);
    write_register(AV_NET_PRB_WAKE_UP, (uint8_t*)&order);
}


bool PR_board::read_is_woken_up() {
    uint32_t rslt(0);
    read_register(AV_NET_PRB_IS_WOKEN_UP, (uint8_t*)&rslt);


    bool prb_woken_up(rslt == AV_NET_CMD_ON);
    Data::get_instance().write(Data::EVENT_PRB_READY, &prb_woken_up);
    return prb_woken_up;
}

void PR_board::clear_to_ignite(uint8_t value) {
    write_register(AV_NET_PRB_CLEAR_TO_IGNITE, &value);
}


void PR_board::read_igniter_oxygen() {
    float pressure(0);
    float temperature(0);

    read_register(AV_NET_PRB_P_OIN, (uint8_t*)&pressure);
    read_register(AV_NET_PRB_T_OIN, (uint8_t*)&temperature);

    Data::get_instance().write(Data::PR_SENSOR_P_OIN, &pressure);
    Data::get_instance().write(Data::PR_SENSOR_T_OIN, &temperature);
}

void PR_board::read_igniter_fuel() {
    float pressure(0);
    float temperature(0);

    read_register(AV_NET_PRB_P_EIN, (uint8_t*)&pressure);
    read_register(AV_NET_PRB_T_EIN, (uint8_t*)&temperature);

    Data::get_instance().write(Data::PR_SENSOR_P_EIN, &pressure);
    Data::get_instance().write(Data::PR_SENSOR_T_EIN, &temperature);
}

void PR_board::read_combution_chamber() {
    float pressure(0);
    float temperature(0);

    read_register(AV_NET_PRB_P_CCC, (uint8_t*)&pressure);
    read_register(AV_NET_PRB_T_CCC, (uint8_t*)&temperature);

    Data::get_instance().write(Data::PR_SENSOR_P_CCC, &pressure);
    Data::get_instance().write(Data::PR_SENSOR_T_CCC, &temperature);
}

void PR_board::write_igniter(uint32_t cmd) {
    write_register(AV_NET_PRB_IGNITER, (uint8_t*)&cmd);
}

void PR_board::write_valves(const uint32_t cmd) {
    write_register(AV_NET_PRB_VALVES_STATE, (uint8_t*)&cmd);
}

void PR_board::read_valves() {
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
}

void PR_board::read_trigger_ack() {
    //uint32_t ack(0);
    //read_register(AV_NET_PRB_IGNITER_ACK, (uint8_t*)&ack);
    //if (ack == AV_NET_CMD_ON) {
    //    // Acknowledgment received
    //}

    //TODO: what should I check for ack ?
    //We could maybe add a Intanet field to PRB to give us feedback on the ignition status
    bool ack(true);
    Data::get_instance().write(Data::EVENT_IGNITED, &ack);

}

void PR_board::execute_abort() {
    none_init_baseHandler(HIGH_PERIOD,delta_ms);
    uint8_t abort_value = AV_NET_CMD_ON;
    write_register(AV_NET_PRB_ABORT, &abort_value);
}

// TODO: Review check policy FLIGHT LOGIC
void PR_board::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    this->delta_ms = delta_ms;
    switch (dump.av_state) {
        case State::INIT:
            // For the INIT state we do nothing
            break;
        case State::ERRORGROUND:
            execute_abort();
            break;
        case State::CALIBRATION:
            // Handle calibration logic
            handle_calibration(dump);
            break;
        case State::DESCENT:
            handle_descent(dump);
            break;
        case State::MANUAL:
            handle_manual(dump);
            break;
        case State::ERRORFLIGHT:
            execute_abort();
            break;
        case State::ARMED:
            handle_armed(dump);
            break;
        case State::READY:
            handle_ready(dump);
            break;
        case State::THRUSTSEQUENCE:
            handle_thrust_sequence(dump);
            break;
        default:
            none_init_baseHandler(HIGH_PERIOD, delta_ms);
            break;
    }
}

void PR_board::handle_error_ground(const DataDump& dump) {
    // Handle errors on the ground
}
void PR_board::handle_calibration(const DataDump& dump) {
        // Write timestamp at a freq of 1Hz
    none_init_baseHandler(HIGH_PERIOD, delta_ms);

    // After DPR GO, send wake_up each 500ms until is_woken_up is true
    if (!dump.event.prb_ready) {
        static uint32_t count_wkp(0);
        static uint8_t wkp_attempts(0);
        count_wkp += delta_ms;

        if (count_wkp >= 500) {
            send_wake_up();
            read_is_woken_up();
            count_wkp = 0;
            ++wkp_attempts;
        }

        if (wkp_attempts > 10) {
        
            // Log error msg
            Logger::log_event(Logger::FATAL, "PRB Board not responding to WAKE_UP command after 10 attempts.");
            // FSM -> ERRORGROUND ?

        }
    }
}

void PR_board::handle_ready(const DataDump& dump) {
    // Write timestamp + clear to trigger at 10Hz
    
    none_init_baseHandler(LOW_PERIOD, delta_ms);
    if(dump.event.ignition_failed) {
        bool trigger_failed = false;
        Data::get_instance().write(Data::EVENT_IGNITION_FAILED, &trigger_failed);
    }
    clear_to_ignite(1);

}


// TODO: Verify the logic with PR
void PR_board::handle_thrust_sequence(const DataDump& dump) {
     static uint32_t trigger_ms(0);
    static uint32_t trigger_ack_ms(0);
    
    if(!dump.event.ignited){
        if (trigger_ms < TIME_ATTEMPT_MS){
            uint32_t trigger(AV_NET_CMD_ON);
            write_igniter(trigger);
            trigger_ms += delta_ms;
        }else{
            //TODO: verify expected to turn off igniter once ignition detected
            uint32_t trigger(AV_NET_CMD_OFF);
            write_igniter(trigger);
            read_trigger_ack();
            trigger_ack_ms += delta_ms;
            //TODO: check ack wait time in Defines (for reviewer)
            if (trigger_ack_ms >= TIMEOUT_MS) {
                // Handle trigger acknowledgment timeout
                Logger::log_event(Logger::ERROR, "Ignition acknowledgment timeout");
                clear_to_ignite(0);
                //back to ready state, write failed in
                bool trigger_failed(true);
                Data::get_instance().write(Data::EVENT_IGNITION_FAILED, &trigger_failed);
                trigger_ms = 0;
                trigger_ack_ms = 0;
            }
        }
    }
}



void PR_board::handle_descent(const DataDump& dump) {
    // Handle logic for descent phase
    //TODO: not final as not needed for VSFT
    none_init_baseHandler(LOW_PERIOD, delta_ms);

    if(dump.event.ignited){
        write_igniter(AV_NET_CMD_OFF);
        uint8_t trigger_off = 0;
        Data::get_instance().write(Data::EVENT_IGNITED, &trigger_off);
    }
}


// TODO: eteindre et allumer chacune des valves
void PR_board::handle_manual(const DataDump& dump) {
    // Process commands in manual mode
    none_init_baseHandler(LOW_PERIOD, delta_ms);
    if (dump.event.command_updated) {
        const uint8_t value(dump.telemetry_cmd.value);
        switch (dump.telemetry_cmd.id) {
            case AV_CMD_MAIN_LOX:
                {
                    // Handle main LOX valve commands
                    uint32_t cmd(0);
                    if (value) {
                        cmd = AV_NET_CMD_ON << AV_NET_SHIFT_MO_BC;
                    }else {
                        cmd = AV_NET_CMD_OFF << AV_NET_SHIFT_MO_BC;
                    }
                    write_valves(cmd); 
                }
                break;
            case AV_CMD_MAIN_FUEL:
                {
                    // Handle main Fuel valve commands
                    uint32_t cmd(0);
                    if (value) {
                        cmd = AV_NET_CMD_ON << AV_NET_SHIFT_ME_B;
                    }else {
                        cmd = AV_NET_CMD_OFF << AV_NET_SHIFT_ME_B;
                    }
                    write_valves(cmd); 
                }
                break;
            default:
                break;
        }
    }
}

