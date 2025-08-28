// TODO: log events

#include <iostream>
#include <pigpio.h>
#include <string>
#include <unistd.h>
#include "data.h"
#include "logger.h"
#include "trigger_board.h"
#include "i2c_interface.h"
#include "intranet_commands.h"

TriggerBoard::TriggerBoard() {
    try {
        I2CInterface::getInstance().open(AV_NET_ADDR_TRB);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf("Error during TRB I2C initilazation: %s", e.what());
    }    
}

TriggerBoard::~TriggerBoard() {
    try {
        I2CInterface::getInstance().close(AV_NET_ADDR_TRB);
    }catch(I2CInterfaceException& e) {
        Logger::log_eventf("Error during TRB I2C deinitialization: %s", e.what());
    }
}

void TriggerBoard::read_register(const uint8_t reg_addr, uint8_t* data) {
    try {
        I2CInterface::getInstance().read(AV_NET_ADDR_TRB, reg_addr, data, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB communication error: failed reading from register ");
        throw TriggerBoardException(msg + std::to_string(reg_addr) + "\n\t" + e.what());
    }
}

void TriggerBoard::write_register(const uint8_t reg_addr, const uint8_t* data) {
    try {
        I2CInterface::getInstance().write(AV_NET_ADDR_TRB, reg_addr, data, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB communication error: failed writing to register ");
        throw TriggerBoardException(msg + std::to_string(reg_addr) + "\n\t" + e.what());
    }
}

void TriggerBoard::write_timestamp() {
    const uint32_t timestamp(Data::get_instance().get().av_timestamp);
    write_register(AV_NET_TRB_TIMESTAMP, (uint8_t*)&timestamp);
}

void TriggerBoard::send_wake_up() {
    const uint32_t order(AV_NET_CMD_ON);
    write_register(AV_NET_TRB_WAKE_UP, (uint8_t*)&order);
}

void TriggerBoard::send_sleep() {
    const uint32_t order(AV_NET_CMD_OFF);
    write_register(AV_NET_TRB_WAKE_UP, (uint8_t*)&order);
}

bool TriggerBoard::read_is_woken_up() {
    unsigned long rslt(0);
    try {
        I2CInterface::getInstance().read(AV_NET_ADDR_TRB, AV_NET_TRB_IS_WOKEN_UP, (uint8_t*)&rslt, AV_NET_XFER_SIZE+1);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB read_is_woken_up error: ");
        throw TriggerBoardException(msg + e.what());
    }

    rslt = (uint32_t)(rslt >> 8);

    bool trb_woken_up(rslt == AV_NET_CMD_ON);
    Data::get_instance().write(Data::EVENT_TRB_READY, &trb_woken_up);

    return trb_woken_up;
}

void TriggerBoard::write_clear_to_trigger(const bool go) {
    const uint32_t cmd(go ? AV_NET_CMD_ON : AV_NET_CMD_OFF);
    write_register(AV_NET_TRB_CLEAR_TO_TRIGGER, (uint8_t*)&cmd);
}

void TriggerBoard::write_pyros(const uint32_t pyros) {
    write_register(AV_NET_TRB_PYROS, (uint8_t*)&pyros);
}

uint32_t TriggerBoard::read_pyros() {
    unsigned long rslt(0);
    try {
	    I2CInterface::getInstance().read(AV_NET_ADDR_TRB, AV_NET_TRB_PYROS, (uint8_t*)&rslt, AV_NET_XFER_SIZE+1);
    }catch(I2CInterfaceException& e) {
	    std::string msg("TRB read_pyros error: ");
	    throw TriggerBoardException(msg + e.what());
    }
    return (uint32_t)(rslt >> 8);
}

bool TriggerBoard::read_has_triggered() {
    unsigned long rslt(0);
    try {
        I2CInterface::getInstance().read(AV_NET_ADDR_TRB, AV_NET_TRB_HAS_TRIGGERED, (uint8_t*)&rslt, AV_NET_XFER_SIZE+1);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB read_has_triggered error: ");
        throw TriggerBoardException(msg + e.what());
    }

    rslt = (uint32_t)(rslt >> 8);
    bool trb_triggered(rslt == AV_NET_CMD_ON);
    Data::get_instance().write(Data::EVENT_SEPERATED, &trb_triggered);

    return trb_triggered;
}

void TriggerBoard::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    this->delta_ms = delta_ms;
    switch (dump.av_state) {
        case State::INIT:
            handle_init();
            break;
        case State::CALIBRATION:
            handle_calibration();
            break;
        case State::FILLING:
            handle_filling();
            break;
        case State::ARMED:
            handle_armed(dump);
            break;
        case State::PRESSURIZED:
            handle_pressurized();
            break;
        case State::IGNITION:
            handle_ignition();
            break;
        case State::BURN:
            handle_burn();
            break;
        case State::ASCENT:
            handle_ascent();
        case State::DESCENT:
            handle_descent(dump);
            break;
        case State::LANDED:
            handle_landed();
            break;
        case State::ABORT_ON_GROUND:
            handle_abort_ground();
            break;
        case State::ABORT_IN_FLIGHT:
            handle_abort_flight();
            break;
    }
}

void TriggerBoard::handle_init() {
    // Write timestamp at a freq of 0.5Hz
    count_ms += delta_ms;
    if (count_ms >= 2000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_calibration() {
    // Write timestamp at a freq of 0.5Hz
    count_ms += delta_ms;
    if (count_ms >= 2000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_filling() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_armed(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }

    // After DPR GO, send wake_up each 500ms until is_woken_up is true
    if (dump.event.dpr_eth_pressure_ok && dump.event.dpr_lox_pressure_ok && !dump.event.trb_ready) {
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
            Logger::log_event(Logger::FATAL, "Trigger Board not responding to WAKE_UP command after 10 attempts.");
            // FSM -> ERRORGROUND ?
        }
    }
}

void TriggerBoard::handle_pressurized() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_ignition() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_burn() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_ascent() {
    // Write timestamp + clear to trigger at 10Hz
    count_ms += delta_ms;
    if (count_ms >= 100) {
        write_timestamp();
        write_clear_to_trigger(1);
    }
}

// Transition ASCENT->DESCENT is done upon apogee detection
// TODO: Confirm firing sequence with ST
void TriggerBoard::handle_descent(const DataDump& dump) {
    static uint32_t trigger_ms(0);
    static uint32_t trigger_ack_ms(0);
    static bool pyro_main_fail(false);
    static bool pyro_spare1_fail(false);
    static bool pyro_spare2_fail(false);

    if (!dump.event.seperated) {
        write_timestamp();

        // Send main pyro order to trigger the sep mech
        if (!pyro_main_fail) {
            if (trigger_ms < 400) {
                uint32_t order(AV_NET_CMD_ON << AV_NET_SHIFT_PYRO1);
                write_pyros(order);
                trigger_ms += delta_ms;
            }else {
                read_has_triggered();
                uint32_t order(AV_NET_CMD_OFF << AV_NET_SHIFT_PYRO1);
                write_pyros(order);

                trigger_ack_ms += delta_ms;
                if (trigger_ack_ms >= 200) {
                    pyro_main_fail = true;
                    trigger_ms = 0;
                    trigger_ack_ms = 0;
                }
            }
        }
        // If passed a delay of no trigger ACK, fire on the spare channels
        else if (!pyro_spare1_fail) {
            if (trigger_ms < 400) {
                uint32_t order(AV_NET_CMD_ON << AV_NET_SHIFT_PYRO2);
                write_pyros(order);
                trigger_ms += delta_ms;
            }else {
                read_has_triggered();
                uint32_t order(AV_NET_CMD_OFF << AV_NET_SHIFT_PYRO2);
                write_pyros(order);

                trigger_ack_ms += delta_ms;
                if (trigger_ack_ms >= 200) {
                    pyro_spare1_fail = true;
                    trigger_ms = 0;
                    trigger_ack_ms = 0;
                }
            }
        }
        // Again, if passed a delay of no trigger ACK, fire on the next channel
        else {
            if (trigger_ms < 400) {
                uint32_t order(AV_NET_CMD_ON << AV_NET_SHIFT_PYRO3);
                write_pyros(order);
                trigger_ms += delta_ms;
            }else {
                read_has_triggered();
                uint32_t order(AV_NET_CMD_OFF << AV_NET_SHIFT_PYRO3);
                write_pyros(order);

                trigger_ack_ms += delta_ms;
                if (trigger_ack_ms >= 200) {
                    pyro_spare2_fail = true;
                    trigger_ms = 0;
                    trigger_ack_ms = 0;
                }
            }
        }
    }else {
        // After separation, write timestamp at 2Hz
        count_ms += delta_ms;
        if (count_ms >= 500) {
            write_timestamp();
            count_ms = 0;
        }
    }
}

void TriggerBoard::handle_landed() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_abort_ground() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_abort_flight() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}


