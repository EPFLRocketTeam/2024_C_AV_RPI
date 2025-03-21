#include <string>
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
    Data::get_instance().write(Data::EVENT_TRB_OK, &trb_woken_up);

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

// TODO: regulate interactions/polling rates
void TriggerBoard::check_policy(const DataDump& dump, const uint32_t delta_ms) {
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
            handle_armed();
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
            handle_descent();
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
    // TODO: write timestamp at a freq of 0.5Hz
    write_timestamp();
}

void TriggerBoard::handle_calibration() {
    // TODO: write timestamp at a freq of 0.5Hz
    write_timestamp();
}

void TriggerBoard::handle_manual() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
}

void TriggerBoard::handle_armed() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
    // TODO: After DPR GO, send wake_up each sec until is_woken_up is true
    static bool trb_woken_up(Data::get_instance().get().event.trb_ok);
    if (!trb_woken_up) {
        send_wake_up();
        read_is_woken_up();
    }
}

void TriggerBoard::handle_ready() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
}

void TriggerBoard::handle_thrustsequence() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
}

void TriggerBoard::handle_liftoff() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
    send_clear_to_trigger();
}

void TriggerBoard::handle_ascent() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
}

// Transition ASCENT->DESCENT is done upon apogee detection
void TriggerBoard::handle_descent() {
    write_timestamp();

    // Send main pyro order to trigger the sep mech
    // TODO: subroutine for timing ON/OFF (usually 300ms but TBD with ST)
    uint32_t order(NET_CMD_ON);
    write_pyros(order);
    // TODO: if passed a delay of no trigger ACK, fire on the spare channels
    read_has_triggered();

    // order = NET_CMD_ON << 8;
    // write_pyros(order);
    // read_has_triggered();
    //
    // order = NET_CMD_ON << 16;
    // write_pyros(order);
    // read_has_triggered();
}

void TriggerBoard::handle_landed() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
}

void TriggerBoard::handle_errorground() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
}

void TriggerBoard::handle_errorflight() {
    // TODO: write timestamp at a freq of 1Hz
    write_timestamp();
}


