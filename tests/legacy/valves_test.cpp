#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include <iostream>

int main() {
	I2CInterface::getInstance().open(AV_NET_ADDR_PRB);
	I2CInterface::getInstance().open(AV_NET_ADDR_DPR_LOX);
	//I2CInterface::getInstance().write(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&test, AV_NET_XFER_SIZE);

	//uint32_t cmd(AV_NET_CMD_ON);
	//I2CInterface::getInstance().write(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&cmd, AV_NET_XFER_SIZE);


	for (int i(0); i < 10; ++i) {
		uint32_t valves(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8);
		std::cout << "Writing valves...";
		I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&valves, AV_NET_XFER_SIZE);
		std::cout << "Done\n";
		AvTimer::sleep(100);
		valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8;
		std::cout << "Writing valves...";
		I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&valves, AV_NET_XFER_SIZE);
		std::cout << "Done\n";
		AvTimer::sleep(100);

		uint32_t valves_dpr(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8 | AV_NET_CMD_ON << 16);
		std::cout << "Writing valves...";
		I2CInterface::getInstance().write(AV_NET_ADDR_DPR_LOX, AV_NET_DPR_VALVES_STATE, (uint8_t*)&valves_dpr, AV_NET_XFER_SIZE);
		std::cout << "Done\n";
		AvTimer::sleep(100);
		valves_dpr = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8 | AV_NET_CMD_OFF << 16;
		std::cout << "Writing valves...";
		I2CInterface::getInstance().write(AV_NET_ADDR_DPR_LOX, AV_NET_DPR_VALVES_STATE, (uint8_t*)&valves_dpr, AV_NET_XFER_SIZE);
		std::cout << "Done\n";
		AvTimer::sleep(100);
	}

//	float value(0);
//	I2CInterface::getInstance().read(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_T_EIN, (uint8_t*)&value, AV_NET_XFER_SIZE);	
//	std::cout << value << "\n";
}
