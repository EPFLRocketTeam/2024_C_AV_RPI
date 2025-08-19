
#ifndef DPR_H
#define DPR_H

#include <exception>
#include <string>
#include "h_driver.h"

class DPR : public HDriver {
public:
    DPR(const uint8_t address);
    virtual ~DPR();

    void write_timestamp(const uint32_t tmsp);
    void wake_up();
    bool read_is_woken_up();
    void send_pressurize();
    void send_abort();
    void read_tank_level();
    void read_tank_pressure();
    void read_tank_temperature();
    void read_copv_pressure();
    void read_copv_temperature();
    void write_valves(const uint32_t cmd);
    void read_valves();

    virtual void check_policy(const DataDump& dump, const uint32_t delta_ms) override;
private:
    uint8_t m_address;
    std::string m_code;
    uint32_t delta_ms;
    uint32_t count_ms;

    void handle_init(const DataDump& dump);
    void handle_calibration(const DataDump& dump);
    void handle_manual(const DataDump& dump);
    void handle_armed(const DataDump& dump);
    void handle_ready(const DataDump& dump);
    void handle_thrustsequence(const DataDump& dump);
    void handle_liftoff(const DataDump& dump);
    void handle_ascent(const DataDump& dump);
    void handle_descent(const DataDump& dump);
    void handle_landed(const DataDump& dump);
    void handle_errorground(const DataDump& dump);
    void handle_errorflight(const DataDump& dump);
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
