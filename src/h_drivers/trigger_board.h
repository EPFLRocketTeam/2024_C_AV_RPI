#ifndef TRIGGER_BOARD_H
#define TRIGGER_BOARD_H

#include <exception>
#include <string>
#include "h_driver.h"

class TriggerBoard : public HDriver {
public:
    TriggerBoard();
    ~TriggerBoard();

    void write_timestamp();
    void send_wake_up();
    void send_sleep();
    bool read_is_woken_up();
    void send_reset();
    void write_clear_to_trigger(const bool go);
    void write_pyros(const uint32_t pyros);    
    uint32_t read_pyros();
    bool read_has_triggered();

    void check_policy(const DataDump& dump, const uint32_t delta_ms) override;

private:
    uint32_t delta_ms;
    uint32_t count_ms;

    void read_register(const uint8_t  reg_addr, uint8_t* data);
    void write_register(const uint8_t reg_addr, const uint8_t* data);

    void handle_init();
    void handle_calibration();
    void handle_filling();
    void handle_armed(const DataDump& dump);
    void handle_pressurized();
    void handle_ignition();
    void handle_burn();
    void handle_ascent();
    void handle_descent(const DataDump& dump);
    void handle_landed();
    void handle_abort_ground();
    void handle_abort_flight();
};

class TriggerBoardException : public std::exception {
public:
    TriggerBoardException(const std::string& msg_) : msg(msg_) {}

    virtual const char* what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
};

#endif /* TRIGGER_BOARD_H */
