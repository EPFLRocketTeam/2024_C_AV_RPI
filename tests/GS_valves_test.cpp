#include "intranet_commands.h"
#include <Protocol.h>
#include "telecom.h"
#include "dpr.h"
#include "data.h"
#include "av_timer.h"
#include "i2c_interface.h"
#include "logger.h"

int main() {
    Logger::init();
	State av_state(State::FILLING);
	Data::get_instance().write(Data::AV_STATE, &av_state);

	//DPR dpr_lox(AV_NET_ADDR_DPR_LOX);
	I2CInterface::getInstance().open(AV_NET_ADDR_PRB);

	Telecom radio;
	radio.begin();

	uint32_t now_ms(AvTimer::tick());
	uint32_t old_ms(0);
	while (1) {
		old_ms = now_ms;
		now_ms = AvTimer::tick();
		uint32_t delta_ms(now_ms - old_ms);
		int timestamp(AvTimer::tick());
		Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp);

		DataDump data(Data::get_instance().get());

		radio.check_policy(data, delta_ms);
		//dpr_lox.check_policy(Data::get_instance().get(), delta_ms);
		//
		Logger::log_eventf("cmd_updated: %i", data.event.command_updated);
		Logger::log_eventf("cmd_id: %i", data.telemetry_cmd.id);

		
		if (data.event.command_updated) {
			uint32_t cmd(0);
			switch (data.telemetry_cmd.id) {
				case CMD_ID::AV_CMD_MAIN_LOX:
					Logger::log_eventf("Received command AV_CMD_MAIN_LOX: %u", cmd);
					if (data.telemetry_cmd.value) {
						cmd = AV_NET_CMD_ON << 8 | AV_NET_CMD_OFF << 0;
					}else {
						cmd = AV_NET_CMD_OFF << 8 | AV_NET_CMD_OFF << 0;
					}
                    Logger::log_eventf("Writing LOX valve to PRB: %x", cmd);
					I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&cmd, AV_NET_XFER_SIZE);
					break;
				case CMD_ID::AV_CMD_MAIN_FUEL:
                    Logger::log_eventf("Received command AV_CMD_MAIN_FUEL: %u", data.telemetry_cmd.value);
					if (data.telemetry_cmd.value) {
						cmd = AV_NET_CMD_ON;
					}else {
						cmd = AV_NET_CMD_OFF;
					}
                    Logger::log_eventf("Writing FUEL valve to PRB: %x", cmd);
					I2CInterface::getInstance().write(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&cmd, AV_NET_XFER_SIZE);
					break;
				default:
					break;
			}
		}

		AvTimer::sleep(100);
		// Read valves state from PRB
		uint32_t prb_valves(0);
		I2CInterface::getInstance().read(AV_NET_ADDR_PRB, AV_NET_PRB_VALVES_STATE, (uint8_t*)&prb_valves, AV_NET_XFER_SIZE);
		uint8_t main_lox((prb_valves & 0x00FF00) >> 8);
		uint8_t main_fuel(prb_valves & 0xFF);

		Valves valves(Data::get_instance().get().valves);

		if (main_lox == AV_NET_CMD_ON) {
			valves.valve_prb_main_lox = 1;
		}else if (main_lox == AV_NET_CMD_OFF) {
			valves.valve_prb_main_lox = 0;
		}

		if (main_fuel == AV_NET_CMD_ON) {
			valves.valve_prb_main_fuel = 1;
		}else if (main_fuel == AV_NET_CMD_OFF) {
			valves.valve_prb_main_fuel = 0;
		}

		Logger::log_eventf("Reading PRB valves: %x", prb_valves);
		Data::get_instance().write(Data::VALVES, &valves);

		AvTimer::sleep(100);
	}
}
