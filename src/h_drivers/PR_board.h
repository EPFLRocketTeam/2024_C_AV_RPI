#ifndef PR_BOARD_H
#define PR_BOARD_H

#include "h_driver.h"
#include <array>
#include <exception>
#include <string>
#include <cstdint>  // For uint8_t

// FIXME: should be deined in intranet_commands.h ?
#define NUM_VALVES 6

class PR_board : public HDriver {
public:
    PR_board();
    ~PR_board();

    void check_policy(const DataDump& dump, const uint32_t delta_ms) override;

    // FIXME: implement the valve logic later on
    // Valve control functions
    // void write_valve(uint8_t valve_id, ValveOpenDegree degree);
    // ValveOpenDegree read_valve(uint8_t valve_id) const;

private:
    // Each element is 4 bits (we store in uint8_t for simplicity, masking the 4 LSB)
    std::array<uint8_t, NUM_VALVES> current_valve_states = {0};

    uint32_t delta_ms;
    uint32_t count_ms;

    void handleErrorFlight(const DataDump& dump);
    void handleErrorGround(const DataDump& dump);
    void handleDescent(const DataDump& dump);
    void processManualMode(const DataDump& dump);
    void handleReady(const DataDump& dump);
    void handleThrustSequence(const DataDump& dump);
    void handleArmed(const DataDump& dump);
    void handleCalibration(const DataDump& dump);
    void read_register(const uint8_t reg_addr, uint8_t* data);
    void write_register(const uint8_t reg_addr, const uint8_t* data);
    void write_timestamp(); 
    void send_wake_up();
    void send_sleep();
    bool read_is_woken_up();
    void clear_to_ignite(uint8_t value);
    void write_trigger(uint32_t trigger);
    void read_trigger_ack();
    void executeAbort();
    void deployRecoverySystem();
    inline void count_time(uint32_t period,uint32_t delta_ms);
    inline void none_init_baseHandler(uint32_t period,uint32_t delta_ms);
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
