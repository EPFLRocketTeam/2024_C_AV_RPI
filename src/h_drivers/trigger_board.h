#ifndef TRIGGER_BOARD_H
#define TRIGGER_BOARD_H

#include "h_driver.h"

class TriggerBoard : public HDriver {
public:
    TriggerBoard();
    ~TriggerBoard();

    void check_policy(Data::GoatReg reg, const DataDump& dump) override;

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

    void write_timestamp();
    void send_wake_up();
    void read_is_woken_up();
    void send_clear_to_trigger();
    void write_pyros(const uint32_t pyros);    
    void read_has_triggered();
};

#endif /* TRIGGER_BOARD_H */
