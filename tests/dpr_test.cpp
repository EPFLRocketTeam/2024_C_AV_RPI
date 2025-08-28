#include <iostream>
#include <cassert>
#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "dpr.h"
#include "logger.h"
#include "data.h"

// Utils functions
void assert_valves(DPR& dpr, const uint32_t valves) {
    dpr.write_valves(valves);
    assert(dpr.read_valves() == valves);
}

void test_valve(DPR& dpr, const uint8_t shift) {
    uint32_t valves(AV_NET_CMD_ON << shift);
    assert_valves(dpr, valves);
    AvTimer::sleep(1e3);
    valves = AV_NET_CMD_OFF << shift;
    assert_valves(dpr, valves);
    AvTimer::sleep(1e3);
}

// TEST FUNCTIONS
void valves_control_test(DPR& dpr) {
    Logger::log_eventf("======VALVES CONTROL TEST======");
    test_valve(dpr, AV_NET_SHIFT_DN_NC);
    test_valve(dpr, AV_NET_SHIFT_PX_NC);
    test_valve(dpr, AV_NET_SHIFT_VX_NO);
}

void throttle_test(DPR& dpr) {
    Logger::log_eventf("=======VALVES THROTTLE TEST======");
	for (int i(0); i < 10; ++i) {
		uint32_t valves(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8  |AV_NET_CMD_ON << 16);
		dpr.write_valves(valves);

		AvTimer::sleep(100);

		valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8 | AV_NET_CMD_OFF << 16;
		dpr.write_valves(valves);

		AvTimer::sleep(100);
	}
}

void read_sensors(DPR& dpr) {
    dpr.read_copv_pressure();
    dpr.read_copv_temperature();
    dpr.read_tank_pressure();
    dpr.read_tank_temperature();
}

void leak_test(DPR& dpr_lox, DPR& dpr_eth) {
    Logger::log_eventf("======LEAK TEST======");
    Logger::log_eventf("Sending ABORT (HOLD) to both DPRs");
    
    dpr_lox.send_abort();
    dpr_eth.send_abort();

    Logger::log_eventf("Starting LOX pressurization in 5s");

    AvTimer::sleep(5e3);

    // Pressurize LOX tank
    dpr_lox.send_pressurize();

    bool eth_pressurized(false);
    uint32_t now_ms(AvTimer::tick());
    uint32_t old_ms(0);
    uint32_t delta_ms(0);
    uint32_t total_ms(0);
    while (1) {
        old_ms = now_ms;
        now_ms = AvTimer::tick();
        delta_ms = now_ms - now_ms;
        total_ms += delta_ms;

        read_sensors(dpr_lox);
        read_sensors(dpr_eth);

        if (total_ms >= 30e3 && !eth_pressurized) {
            dpr_eth.send_pressurize();
            eth_pressurized = true;
        }

        AvTimer::sleep(200);
    }
}

int main(int argc, char** argv) {
	Logger::init();
	DPR dpr_lox(AV_NET_ADDR_DPR_LOX);
    DPR dpr_eth(AV_NET_ADDR_DPR_ETH);
    
	uint32_t timestamp(12345);
    Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp);
	dpr_lox.write_timestamp();
    dpr_eth.write_timestamp();
	
    valves_control_test(dpr_lox);
    read_sensors(dpr_lox);

    valves_control_test(dpr_eth);
    read_sensors(dpr_eth);

    Logger::terminate();
}
