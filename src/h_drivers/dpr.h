
#ifndef DPR_H
#define DPR_H

#include <exception>
#include <string>
#include "h_driver.h"

class DPR : public HDriver {
public:
    DPR(const uint8_t address);
    virtual ~DPR();

    // I2C registers R/W low-level functions
    void write_timestamp();
    void wake_up();
    bool read_is_woken_up();
    void send_pressurize(const bool active);
    void send_passivate();
    void send_reset();
    void send_abort(const bool in_flight);
    float read_tank_pressure();
    float read_tank_temperature();
    float read_copv_pressure();
    float read_copv_temperature();
    void write_valves(const uint32_t cmd);
    uint32_t read_valves();

    virtual void check_policy(const DataDump& dump, const uint32_t delta_ms) override;
private:
    uint8_t m_address;
    std::string m_code;
    uint32_t delta_ms;
    uint32_t count_ms;
    uint32_t passivation_count_ms;

    void handle_init(const DataDump& dump);
    void handle_calibration(const DataDump& dump);
    void handle_filling(const DataDump& dump);
    void handle_armed(const DataDump& dump);
    void handle_pressurization(const DataDump& dump);
    void handle_ignition(const DataDump& dump);
    void handle_burn(const DataDump& dump);
    void handle_ascent(const DataDump& dump);
    void handle_descent(const DataDump& dump);
    void handle_landed(const DataDump& dump);
    void handle_abort_ground(const DataDump& dump);
    void handle_abort_flight(const DataDump& dump);

    void listen_valves_command(const DataDump& dump);
    inline void periodic_timestamp(const uint32_t period);
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
