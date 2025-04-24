// TODO: log events

#include <string>
#include <unistd.h>
#include "data.h"
#include "trigger_board.h"
#include "i2c_interface.h"
#include "intranet_commands.h"

TriggerBoard::TriggerBoard() {
    try {
        I2CInterface::getInstance().open(NET_ADDR_TRB);
    }catch(const I2CInterfaceException& e) {
        std::cout << "Error during TRB I2C initilazation: " << e.what() << "\n";
    }    
}

TriggerBoard::~TriggerBoard() {
    try {
        I2CInterface::getInstance().close(NET_ADDR_TRB);
    }catch(I2CInterfaceException& e) {
        std::cout << "Error during TRB I2C deinitialization: " << e.what() << "\n";
    }
}

void TriggerBoard::write_timestamp() {
    const uint32_t timestamp(Data::get_instance().get().av_timestamp);
    try {
        I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_TIMESTAMP_MAIN, (uint8_t*)&timestamp,
        NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB write_timestamp error: ");
        throw TriggerBoardException(msg + e.what());
    }
}

void TriggerBoard::send_wake_up() {
    const uint32_t order(NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_WAKE_UP, (uint8_t*)&order, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB wake_up error: ");
        throw TriggerBoardException(msg + e.what());
    }
}

bool TriggerBoard::read_is_woken_up() {
    uint32_t rslt(0);
    try {
        I2CInterface::getInstance().read(NET_ADDR_TRB, TRB_IS_WOKEN_UP, (uint8_t*)&rslt, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB read_is_woken_up error: ");
        throw TriggerBoardException(msg + e.what());
    }

    bool trb_woken_up(rslt == NET_CMD_ON);
    Data::get_instance().write(Data::EVENT_TRB_READY, &trb_woken_up);

    return trb_woken_up;
}

void TriggerBoard::send_clear_to_trigger() {
    const uint32_t cmd(NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_CLEAR_TO_TRIGGER, (uint8_t*)&cmd, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB clear_to_trigger error: ");
        throw TriggerBoardException(msg + e.what());
    }
}

void TriggerBoard::write_pyros(const uint32_t pyros) {
    try {
        I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_PYROS, (uint8_t*)&pyros, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB write_pyros error: ");
        throw TriggerBoardException(msg + e.what());
    }
}

bool TriggerBoard::read_has_triggered() {
    uint32_t rslt(0);
    try {
        I2CInterface::getInstance().read(NET_ADDR_TRB, TRB_HAS_TRIGGERED, (uint8_t*)&rslt, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("TRB read_has_triggered error: ");
        throw TriggerBoardException(msg + e.what());
    }

    bool trb_triggered(rslt == NET_CMD_ON);
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
        case State::MANUAL:
            handle_manual();
            break;
        case State::ARMED:
            handle_armed(dump);
            break;
        case State::READY:
            handle_ready();
            break;
        case State::THRUSTSEQUENCE:
            handle_thrustsequence();
            break;
        case State::LIFTOFF:
            handle_liftoff();
            break;
        case State::ASCENT:
            handle_ascent();
        case State::DESCENT:
            handle_descent(dump);
            break;
        case State::LANDED:
            handle_landed();
            break;
        case State::ERRORGROUND:
            handle_errorground();
            break;
        case State::ERRORFLIGHT:
            handle_errorflight();
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

void TriggerBoard::handle_manual() {
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
            // FSM -> ERRORGROUND ?
        }
    }
}

void TriggerBoard::handle_ready() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_thrustsequence() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_liftoff() {
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
        send_clear_to_trigger();
    }
}

// Transition ASCENT->DESCENT is done upon apogee detection
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
                uint32_t order(NET_CMD_ON);
                write_pyros(order);
                trigger_ms += delta_ms;
            }else {
                read_has_triggered();
                uint32_t order(NET_CMD_OFF);
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
                uint32_t order(NET_CMD_ON << 8);
                write_pyros(order);
                trigger_ms += delta_ms;
            }else {
                read_has_triggered();
                uint32_t order(NET_CMD_OFF << 8);
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
                uint32_t order(NET_CMD_ON << 16);
                write_pyros(order);
                trigger_ms += delta_ms;
            }else {
                read_has_triggered();
                uint32_t order(NET_CMD_OFF << 16);
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

void TriggerBoard::handle_errorground() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}

void TriggerBoard::handle_errorflight() {
    // Write timestamp at a freq of 1Hz
    count_ms += delta_ms;
    if (count_ms >= 1000) {
        write_timestamp();
        count_ms = 0;
    }
}


