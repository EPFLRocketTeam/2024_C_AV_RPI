/**
 * @file    trb_test.cpp
 * @author  Cyprien Lacassagne
 * @brief   FC (M) - TRB (S) I2C Communication Test
 */

#include <iostream>
#include <cassert>
#include <unistd.h>
#include "av_state.h"
#include "data.h"
#include "intranet_commands.h"
#include "trigger_board.h"
#include "logger.h"
#include <fstream>

void read_write_test(TriggerBoard& trb);
void check_policy_test(TriggerBoard& trb);

int main() {
    std::ofstream log("/boot/av_log/trb_main_test.log", std::ios::app);
    log << "init test" << std::endl;

    DataLogger::getInstance().eventConv("TRB test", 0);
    log << "\x1b[7m" "Avionics Trigger Board I2C Tests" "\x1b[0m\n\n";

    TriggerBoard trb;
    DataLogger::getInstance().eventConv("TRB init", 0);
    log << "Testing R/W op. and TRB behavior\n";
    read_write_test(trb);

    //std::cout << "Testing driver policy\n";
    //check_policy_test(trb);
    log.close();
}

void read_write_test(TriggerBoard& trb) {
    std::ofstream log("/boot/av_log/trb_write_test.log", std::ios::app);
    // TRB is in SLEEP mode until we send WAKEUP
    DataLogger::getInstance().eventConv(" - Checking TRB is in SLEEP mode...", 0);
    log << " - Checking TRB is in SLEEP mode... ";
    assert(trb.read_is_woken_up() == false);
    log << "\x1b[32mOK\x1b[0m\n";
    DataLogger::getInstance().eventConv(" - Checking TRB triggered flag is not set ...", 0);
    log << " - Checking TRB triggered flag is not set... ";
    assert(trb.read_has_triggered() == false);
    log << "\x1b[32mOK\x1b[0m\n";

    log << " - Writing FSM timestamp to TRB... ";
    trb.send_wake_up();
    log << "\x1b[32mOK\x1b[0m\n";

    log << " - Sending WAKEUP order... ";
    trb.send_wake_up();
    log << "\x1b[32mOK\x1b[0m\n";

    log << " - Waiting 100ms... ";
    usleep(100e3);
    log << "Done\n";

    log << " - Checking TRB is woken up... ";
    assert(trb.read_is_woken_up() == true);
    log << "\x1b[32mOK\x1b[0m\n";

    log << " - Checking TRB triggered flag is not set... ";
    assert(trb.read_has_triggered() == false);
    log << "\x1b[32mOK\x1b[0m\n";

    log << " - Sending PYRO NET_CMD_ON... ";
    uint32_t cmd(NET_CMD_ON);
    trb.write_pyros(cmd);
    log << "\x1b[32mOK\x1b[0m\n";

    log << " - Waiting 300ms...";
    usleep(300e3);
    log << "Done\n";

    log << " - Sending PYRO NET_CMD_OFF... ";
    cmd = NET_CMD_OFF;
    trb.write_pyros(cmd);
    log << "\x1b[32mOK\x1b[0m\n";

    log << " - Checking TRB has triggered... ";
    assert(trb.read_has_triggered() == true);
    log << "\x1b[32mOK\x1b[0m\n";

    log << "\x1b[32m\x1b[7mI2C R/W SUCCESS\x1b[0m\n\n";
    log.close();
}

void check_policy_test(TriggerBoard& trb) {
    DataDump goat;

    uint8_t state(static_cast<uint8_t>(State::INIT));

    while (state < static_cast<uint8_t>(State::ERRORFLIGHT)) {
        goat.av_state = static_cast<State>(state); 
        trb.check_policy(goat, 0);
        ++state;

        usleep(5e5);
    }
}
