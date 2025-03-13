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
            write_timestamp();
            break;
        case State::CALIBRATION:
            write_timestamp();
            // TODO: send wake up until is_woken_up is true
            send_wake_up();
            read_is_woken_up();
            break;
        case State::MANUAL:
        case State::ARMED:
        case State::READY:
        case State::THRUSTSEQUENCE:
            write_timestamp();
            break;
        case State::LIFTOFF:
            write_timestamp();
            send_clear_to_trigger();
            break;
        case State::ASCENT:
        case State::LANDED:
            write_timestamp();
            break;
        case State::DESCENT:
            write_timestamp();
            read_has_triggered();
            break;
        case State::ERRORGROUND:
        case State::ERRORFLIGHT:
            write_timestamp();
            break;
    }
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


