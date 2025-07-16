/**
 * @file    trb_test.cpp
 * @author  Cyprien Lacassagne
 * @brief   FC (M) - TRB (S) I2C Communication Test
 */

#include <iostream>
#include <cassert>
#include <unistd.h>
#include <random>
#include "av_state.h"
#include "data.h"
#include "intranet_commands.h"
#include "trigger_board.h"

void intensive_read_write_test(TriggerBoard& trb);
void sequential_read_write_test(TriggerBoard& trb);
void check_policy_test(TriggerBoard& trb);

int main() {
    srand((unsigned) time(0));

    std::cout << "\x1b[7m" "Avionics Trigger Board I2C Tests" "\x1b[0m\n\n";

    TriggerBoard trb;

    std::cout << "Trigger Board driver initialized successfully\n";
    
    std::cout << "Testing intensive R/W operations and coherence:\n";
    intensive_read_write_test(trb);
    std::cout << "\n\n";
    std::cout << "Testing R/W op. and TRB behavior:\n";
    sequential_read_write_test(trb);

    //std::cout << "Testing driver policy\n";
    //check_policy_test(trb);
}

void intensive_read_write_test(TriggerBoard& trb) {
    const uint8_t iter(128);
    for (int i(0); i < iter; ++i) {
	uint32_t tmsp(rand());
	Data::get_instance().write(Data::AV_TIMESTAMP, &tmsp);
	trb.write_timestamp();
	usleep(10e3);
    }
    for (int i(0); i < iter; ++i) {
	trb.read_is_woken_up();
	usleep(10e3);
    }
    for (int i(0); i < iter; ++i) {
	trb.read_has_triggered();
	usleep(10e3);
    }
    for (int i(0); i < iter; ++i) {
	const uint32_t val(rand());
	std::cout << val << "\t";
	trb.write_pyros(val);
	usleep(10e3);
	std::cout << trb.read_pyros() << "\n";
	//assert(trb.read_pyros() == val);
	//usleep(10e3);
	//assert(trb.read_has_triggered() == false);
	//usleep(10e3);
    }
}

void sequential_read_write_test(TriggerBoard& trb) {
    // TRB is in SLEEP mode until we send WAKEUP
    std::cout << " - Checking TRB is in SLEEP mode... ";
    assert(trb.read_is_woken_up() == false);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Checking TRB triggered flag is not set... ";
    assert(trb.read_has_triggered() == false);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Writing FSM timestamp to TRB... ";
    uint32_t tmsp(341400);
    Data::get_instance().write(Data::AV_TIMESTAMP, &tmsp);
    trb.write_timestamp();
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Waiting 1s...\n";
    usleep(1e6);

    std::cout << " - Sending WAKEUP order... ";
    trb.send_wake_up();
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Waiting 5s...\n";
    usleep(5e6);

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

    std::cout << " - Waiting 300ms...\n";
    usleep(300e3);

    std::cout << " - Sending PYRO_2 NET_CMD_OFF... ";
    cmd = NET_CMD_OFF << 8;
    trb.write_pyros(cmd);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Checking TRB has not triggered... ";
    assert(trb.read_has_triggered() == false);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Waiting 1s...\n";
    usleep(1e6);

    std::cout << " - Sending CLEAR_TO_TRIGGER order... ";
    trb.send_clear_to_trigger();
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Waiting 5s...\n";
    usleep(5e6);

    std::cout << " - Sending PYRO_2 NET_CMD_ON... ";
    cmd = NET_CMD_ON << 8;
    trb.write_pyros(cmd);
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << " - Waiting 300ms...\n";
    usleep(300e3);

    std::cout << " - Sending PYRO1 NET_CMD_OFF... ";
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
