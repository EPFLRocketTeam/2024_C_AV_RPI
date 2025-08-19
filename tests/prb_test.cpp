#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "logger.h"
#include <iostream>

int main() {
	I2CInterface::getInstance().open(AV_NET_ADDR_PRB);
	uint32_t test(0xFFFFFFFF);
	//I2CInterface::getInstance().write(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&test, AV_NET_XFER_SIZE);

	//uint32_t cmd(AV_NET_CMD_ON);
	//I2CInterface::getInstance().write(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&cmd, AV_NET_XFER_SIZE);


	/*
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
	}
	*/

	//test = AV_NET_CMD_ON;
	//I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&test, AV_NET_XFER_SIZE);

	
	/*
	float rslt(0);
	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_T_CCC, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
	Logger::log_eventf("Reading T_CCC: %f\n", rslt);
	*/


	uint32_t valves_write(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8);
	Logger::log_eventf("Writing valves: %x\n", valves_write);
	I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&valves_write, AV_NET_XFER_SIZE);

	
	uint32_t valves(0);
	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&valves, AV_NET_XFER_SIZE);
	Logger::log_eventf("Reading valves: %x\n", valves);
	

	/*
	uint32_t woken_up(0);
	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_IS_WOKEN_UP, (uint8_t*)&woken_up, AV_NET_XFER_SIZE);
	Logger::log_eventf("Reading woken_up: %u | %b\n", woken_up, woken_up);
*/
//	float value(0);
//	I2CInterface::getInstance().read(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_T_EIN, (uint8_t*)&value, AV_NET_XFER_SIZE);	
//	std::cout << value << "\n";
}
