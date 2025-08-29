#include <iostream>
#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "logger.h"
#include "PR_board.h"

void throttle_test(PR_board& prb) {
    for (int i(0); i < 10; ++i) {
        uint32_t valves(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8);
        prb.write_valves(valves); 

        AvTimer::sleep(100);

        valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8;
        prb.write_valves(valves);

        AvTimer::sleep(100);
    }
}

int main() {
	Logger::init();
    PR_board prb;

    // Valves throttle test
    throttle_test(prb);

	uint32_t cmd(0);
	cmd = AV_NET_CMD_ON;

    prb.send_wake_up();
    prb.read_is_woken_up();
    prb.write_igniter(cmd);
    prb.clear_to_ignite(cmd);
    AvTimer::sleep(1e3);
    prb.write_igniter(AV_NET_CMD_ON);

/*
	AvTimer::sleep(500);
    prb.write_igniter(cmd);
	Logger::log_eventf(Logger::DEBUG, "Writing IGNITER");
*/	

	float rslt(0);
    prb.read_combustion_chamber();
    prb.read_injector_oxygen();
    prb.read_injector_fuel();

	uint32_t valves(0);
    prb.read_valves();

    prb.read_combustion_chamber();

	/*
	uint32_t woken_up(0);
	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_IS_WOKEN_UP, (uint8_t*)&woken_up, AV_NET_XFER_SIZE);
	Logger::log_eventf("Reading woken_up: %u | %b", woken_up, woken_up);
*/
//	float value(0);
//	I2CInterface::getInstance().read(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_T_EIN, (uint8_t*)&value, AV_NET_XFER_SIZE);	
//	std::cout << value << "\n";

	Logger::terminate();
}
