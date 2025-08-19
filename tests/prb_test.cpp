#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "logger.h"
#include <iostream>

int main() {
	Logger::init();
	I2CInterface::getInstance().open(AV_NET_ADDR_PRB);
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

	uint32_t cmd(0);
	cmd = AV_NET_CMD_ON;

	I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_WAKE_UP, (uint8_t*)&cmd, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Writing WAKE_UP");

	uint32_t value(0);
	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_IS_WOKEN_UP, (uint8_t*)&value, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading IS_WOKEN_UP: %x", value);

	I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_IGNITER, (uint8_t*)&cmd, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Writing IGNITER");

	I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&cmd, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Writing CLEAR_TO_IGNITE");
/*
	AvTimer::sleep(500);
	I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_IGNITER, (uint8_t*)&cmd, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Writing IGNITER");
*/	

	float rslt(0);
	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_T_CCC, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading T_CCC: %f", rslt);

	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_P_CCC, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading P_CCC: %f", rslt);

	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_T_OIN, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading T_OIN: %f", rslt);

	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_P_OIN, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading P_OIN: %f", rslt);

	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_P_EIN, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading P_EIN: %f", rslt);

	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_T_EIN, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading T_EIN: %f", rslt);


	uint32_t valves_write(AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8);
	Logger::log_eventf(Logger::DEBUG, "Writing valves: %x", valves_write);
	I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&valves_write, AV_NET_XFER_SIZE);

	
	uint32_t valves(0);
	I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&valves, AV_NET_XFER_SIZE);
	Logger::log_eventf(Logger::DEBUG, "Reading valves: %x", valves);
	

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
