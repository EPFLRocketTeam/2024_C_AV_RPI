#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "dpr.h"
#include "logger.h"
#include <iostream>

const uint8_t address_lox(AV_NET_ADDR_DPR_LOX);
const uint8_t address_eth(AV_NET_ADDR_DPR_ETH);

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

int main(int argc, char** argv) {
	Logger::init();
	DPR dpr_lox(address_lox);
    DPR dpr_eth(address_eth);
    
    
    Logger::log_eventf("======LEAK TEST======");
    Logger::log_eventf("Sending ABORT (HOLD) to both DPRs");
    
    //dpr_lox.send_abort();
    //dpr_eth.send_abort();

    Logger::log_eventf("Starting LOX pressurization in 5s");

    AvTimer::sleep(5e3);

    // Pressurize LOX tank
    //dpr_lox.send_pressurize();

    bool eth_pressurized(false);
    uint32_t now_ms(AvTimer::tick());
    uint32_t old_ms(0);
    uint32_t delta_ms(0);
    uint32_t total_ms(0);
    while (1) {
        old_ms = now_ms;
        now_ms = AvTimer::tick();
        delta_ms = old_ms - now_ms;
        total_ms += delta_ms;

        dpr_lox.read_tank_pressure();
        dpr_lox.read_tank_temperature();
        dpr_lox.read_copv_pressure();
        dpr_lox.read_copv_temperature();
        dpr_eth.read_tank_pressure();
        dpr_eth.read_tank_temperature();
        dpr_eth.read_copv_pressure();
        dpr_eth.read_copv_temperature();

        if (total_ms >= 30e3 && !eth_pressurized) {
            //dpr_eth.send_pressurize();
            eth_pressurized = true;
        }

        AvTimer::sleep(200);
    }

    //throttle_test(dpr_lox);
    /*
	uint32_t timestamp(12345);
	dpr_lox.write_timestamp();
	
	uint32_t valves(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8  |AV_NET_CMD_ON << 16);
	dpr_lox.write_valves(valves);
	dpr_lox.read_valves();

	AvTimer::sleep(1e3);

	valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8  |AV_NET_CMD_OFF << 16;
	dpr_lox.write_valves(valves);
	dpr_lox.read_valves();

    float sensor(0);
    I2CInterface::getInstance().read(address, AV_NET_DPR_T_NCO, (uint8_t*)&sensor, AV_NET_XFER_SIZE);
    Logger::log_eventf(Logger::DEBUG, "Reading T_NCO: %f", sensor);

    I2CInterface::getInstance().read(address, AV_NET_DPR_P_NCO, (uint8_t*)&sensor, AV_NET_XFER_SIZE);
    Logger::log_eventf(Logger::DEBUG, "Reading P_NCO: %f", sensor);

    I2CInterface::getInstance().read(address, AV_NET_DPR_T_XTA, (uint8_t*)&sensor, AV_NET_XFER_SIZE);
    Logger::log_eventf(Logger::DEBUG, "Reading T_XTA: %f", sensor);

    I2CInterface::getInstance().read(address, AV_NET_DPR_P_XTA, (uint8_t*)&sensor, AV_NET_XFER_SIZE);
    Logger::log_eventf(Logger::DEBUG, "Reading P_XTA: %f", sensor);
    */
}
