#ifndef TRIGGER_BOARD_H
#define TRIGGER_BOARD_H

#include "h_driver.h"
#include <exception>
#include <string>

class TriggerBoard : public HDriver {
public:
    TriggerBoard();
    ~TriggerBoard();

    void write_timestamp();
    void send_wake_up();
    bool read_is_woken_up();
    void send_clear_to_trigger();
    void write_pyros(const uint32_t pyros);    
    bool read_has_triggered();

    void check_policy(const DataDump& dump) override;

private:
    void handle_init();
    void handle_calibration();
    void handle_manual();
    void handle_armed();
    void handle_ready();
    void handle_thrustsequence();
    void handle_liftoff();
    void handle_ascent();
    void handle_descent();
    void handle_landed();
    void handle_errorground();
    void handle_errorflight();
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
