#include "trigger_board.h"
#include "i2c_interface.h"
#include "intranet_commands.h"

TriggerBoard::TriggerBoard() {
    try {
        I2CInterface::getInstance().open(NET_ADDR_TRB);
    }catch(const I2CInterfaceException& e) {
        std::cout << "Error during TRB I2C initilazation:" << e.what() << "\n";
    }    
}

TriggerBoard::~TriggerBoard() {
    try {
        I2CInterface::getInstance().close(NET_ADDR_TRB);
    }catch(I2CInterfaceException& e) {
        std::cout << "Error during TRB I2C deinitialization: " << e.what() << "\n";
    }
}

// TODO: regulate interactions/polling rates
void TriggerBoard::check_policy(Data::GoatReg reg, const DataDump& dump) {
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
    write_timestamp();
}

void TriggerBoard::handle_calibration() {
    write_timestamp();
    // TODO: send wake up until is_woken_up id true
    send_wake_up();
    read_is_woken_up();
}

void TriggerBoard::handle_manual() {
    write_timestamp();
}

void TriggerBoard::handle_armed() {
    write_timestamp();
}

void TriggerBoard::handle_ready() {
    write_timestamp();
}

void TriggerBoard::handle_thrustsequence() {
    write_timestamp();
}

void TriggerBoard::handle_liftoff() {
    write_timestamp();
}

void TriggerBoard::handle_ascent() {
    write_timestamp();
}

// Transition ASCENT->DESCENT is done upon apogee detection
void TriggerBoard::handle_descent() {
    write_timestamp();

    // Send main pyro order to trigger the sep mech
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
    write_timestamp();
}

void TriggerBoard::handle_errorground() {
    write_timestamp();
}

void TriggerBoard::handle_errorflight() {
    write_timestamp();
}

void TriggerBoard::write_timestamp() {
    // TODO: Need to add main timestamp to the GOAT.
//    I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_TIMESTAMP_MAIN, 
}

void TriggerBoard::send_wake_up() {
    const uint32_t order(NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_WAKE_UP, (uint8_t*)&order, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::cout << "TRB communication error: " << e.what() << "\n";
    }
}

void TriggerBoard::read_is_woken_up() {
    uint32_t rslt(0);
    try {
        I2CInterface::getInstance().read(NET_ADDR_TRB, TRB_IS_WOKEN_UP, (uint8_t*)&rslt, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::cout << "TRB communication error: " << e.what() << "\n";
    }
    // TODO: Write rslt to the GOAT (need to add woken_up entries to the GOAT for TRB and PRB)
}

void TriggerBoard::send_clear_to_trigger() {
    const uint32_t cmd(NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_CLEAR_TO_TRIGGER, (uint8_t*)&cmd, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::cout << "TRB communication error: " << e.what() << "\n";
    }
}

void TriggerBoard::write_pyros(const uint32_t pyros) {
    try {
        I2CInterface::getInstance().write(NET_ADDR_TRB, TRB_PYROS, (uint8_t*)&pyros, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::cout << "TRB communication error: " << e.what() << "\n";
    }
}

void TriggerBoard::read_has_triggered() {
    uint32_t rslt(0);
    try {
        I2CInterface::getInstance().read(NET_ADDR_TRB, TRB_HAS_TRIGGERED, (uint8_t*)&rslt, NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::cout << "TRB communication error: " << e.what() << "\n";
    }
}


