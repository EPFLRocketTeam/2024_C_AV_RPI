
#ifndef DPR_H
#define DPR_H

#include <exception>
#include "h_driver.h"

class DPR : public HDriver {
public:
    DPR(const uint8_t address);
    virtual ~DPR();

    void write_timestamp();
    void wake_up();
    bool read_is_woken_up();
    void send_pressurize();
    void send_abort();
    void read_tank_level();
    void read_tank_pressure();
    void read_tank_temperature();
    void read_copv_pressure();
    void read_copv_temperature();
    void write_tank_valve(const uint8_t cmd);
    void write_copv_valve(const uint8_t cmd);

    virtual void check_policy(const DataDump& dump, const uint32_t delta_ms) override;
private:
    uint8_t m_address;
    std::string m_code;
    uint32_t delta_ms;

    void handle_init();
    void handle_calibration();
    void handle_manual(const DataDump& dump);
    void handle_armed(const DataDump& dump);
    void handle_ready();
    void handle_thrustsequence();
    void handle_liftoff();
    void handle_ascent();
    void handle_descent();
    void handle_landed();
    void handle_errorground();
    void handle_errorflight();
};

class DPRException : public std::exception {
public:
    DPRException(const std::string& msg_) : msg(msg_) {}

    virtual const char* what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg;
};

#endif /* DPR_H */
