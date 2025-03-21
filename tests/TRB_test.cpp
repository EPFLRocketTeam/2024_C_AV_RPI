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

void read_write_test(TriggerBoard& trb);
void check_policy_test(TriggerBoard& trb);

int main() {
    std::cout << "\x1b[7m" "Avionics Trigger Board I2C Tests" "\x1b[0m\n\n";

    TriggerBoard trb;
    
    std::cout << "Testing R/W op. and TRB behavior\n";
    read_write_test(trb);

    //std::cout << "Testing driver policy\n";
    //check_policy_test(trb);
}

void read_write_test(TriggerBoard& trb) {
    // TRB is in SLEEP mode until we send WAKEUP
    std::cout << " - Checking TRB is in SLEEP mode... ";
    assert(trb.read_is_woken_up() == false);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Checking TRB triggered flag is not set... ";
    assert(trb.read_has_triggered() == false);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Writing FSM timestamp to TRB... ";
    trb.send_wake_up();
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Sending WAKEUP order... ";
    trb.send_wake_up();
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Waiting 100ms... ";
    usleep(100e3);
    std::cout << "Done\n";

    std::cout << " - Checking TRB is woken up... ";
    assert(trb.read_is_woken_up() == true);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Checking TRB triggered flag is not set... ";
    assert(trb.read_has_triggered() == false);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Sending PYRO NET_CMD_ON... ";
    uint32_t cmd(NET_CMD_ON);
    trb.write_pyros(cmd);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Waiting 300ms...";
    usleep(300e3);
    std::cout << "Done\n";

    std::cout << " - Sending PYRO NET_CMD_OFF... ";
    cmd = NET_CMD_OFF;
    trb.write_pyros(cmd);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Checking TRB has triggered... ";
    assert(trb.read_has_triggered() == true);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << "\x1b[32m\x1b[7mI2C R/W SUCCESS\x1b[0m\n\n";
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
