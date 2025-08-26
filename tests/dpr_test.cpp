#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "dpr.h"
#include "logger.h"
#include <iostream>

void throttle_test(DPR& dpr) {
	for (int i(0); i < 10; ++i) {
		uint32_t valves(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8  |AV_NET_CMD_ON << 16);
		dpr.write_valves(valves);

		AvTimer::sleep(100);

		valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8 | AV_NET_CMD_OFF << 16;
		dpr.write_valves(valves);

		AvTimer::sleep(100);
	}
}

int main() {
	Logger::init();
	DPR dpr_lox(AV_NET_ADDR_DPR_LOX);

    throttle_test(dpr_lox);

	uint32_t timestamp(12345);
	dpr_lox.write_timestamp(timestamp);
	
	uint32_t valves(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8  |AV_NET_CMD_ON << 16);
	dpr_lox.write_valves(valves);
	dpr_lox.read_valves();

	AvTimer::sleep(1e3);

	valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8  |AV_NET_CMD_OFF << 16;
	dpr_lox.write_valves(valves);
	dpr_lox.read_valves();

    float sensor(0);
    I2CInterface::getInstance().read(AV_NET_ADDR_DPR_LOX, AV_NET_DPR_T_NCO, (uint8_t*)&sensor, AV_NET_XFER_SIZE);
    Logger::log_eventf(Logger::DEBUG, "Reading T_NCO: %f", sensor);

    I2CInterface::getInstance().read(AV_NET_ADDR_DPR_LOX, AV_NET_DPR_T_NCO, (uint8_t*)&sensor, AV_NET_XFER_SIZE);
    Logger::log_eventf(Logger::DEBUG, "Reading T_NCO: %f", sensor);
}
