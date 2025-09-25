#ifndef PR_BOARD_H
#define PR_BOARD_H

#include "h_driver.h"
#include <array>
#include <exception>
#include <string>
#include <cstdint>  // For uint8_t

class PR_board : public HDriver {
public:
    PR_board();
    ~PR_board();

    void check_policy(const DataDump& dump, const uint32_t delta_ms) override;
    void actuate_valve(const bool active, const uint8_t valve_bitshift);

    // FIXME: implement the valve logic later on
    // Valve control functions
    // void write_valve(uint8_t valve_id, ValveOpenDegree degree);
    // ValveOpenDegree read_valve(uint8_t valve_id) const;
    void write_timestamp(); 
    void send_wake_up();
    void send_sleep();
    void send_reset();
    bool read_is_woken_up();
    void clear_to_ignite(bool value);
    void send_passivate();
    void read_fsm();
    void read_injector_oxygen();
    void read_injector_fuel();
    void read_injector_cooling_temperature();
    void read_combustion_chamber();
    void write_igniter(uint32_t cmd);
    void write_valves(const uint32_t cmd);
    uint32_t read_valves();
    float read_pressure_check();
    float read_impulse(const DataDump& dump);

private:
    uint32_t delta_ms;
    uint32_t count_ms;
    uint32_t polling_count_ms;

    // Internal states and counters
    uint32_t ignition_send_ms;
    uint32_t ignition_ack_ms;
    bool ignition_sq_started;
    bool ignited;
    uint32_t burn_elapsed_ms;
    uint32_t passivation_count_ms;
    void reset_counters();

    void handle_init(const DataDump& dump);
    void handle_calibration(const DataDump& dump);
    void handle_filling(const DataDump& dump);
    void handle_armed(const DataDump& dump);
    void handle_pressurization(const DataDump& dump);
    void handle_ignition(const DataDump& dump);
    void handle_burn(const DataDump& dump);
    void handle_ascent(const DataDump& dump);
    void handle_descent(const DataDump& dump);
    void handle_abort_flight(const DataDump& dump);
    void handle_abort_ground(const DataDump& dump);

    void read_register(const uint8_t reg_addr, uint8_t* data);
    void write_register(const uint8_t reg_addr, const uint8_t* data);

    void listen_valves_command(const DataDump& dump);
    inline void count_time(uint32_t period,uint32_t delta_ms);
    inline void periodic_timestamp(const uint32_t period);
};

class PRBoardException : public std::exception {
public:
    PRBoardException(const std::string& msg_) : msg(msg_) {}

    virtual const char* what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
};

#endif /* PR_BOARD_H */
