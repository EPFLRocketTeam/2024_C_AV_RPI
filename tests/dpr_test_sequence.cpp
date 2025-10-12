#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "dpr.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include "data.h"
#include <vector>

#ifndef DPR_TEST_SEQUENCE
#define DPR_TEST_SEQUENCE "dpr_test_sequence.txt"
#endif

DPR* dpr = nullptr;
uint32_t next_op = 0;

struct SequenceOperation {
	virtual void apply () = 0;
};
struct SleepOperation : SequenceOperation {
	int duration;

	SleepOperation (int duration) : duration(duration) {}

	void apply () override {
		AvTimer::sleep(duration);
	}
};
struct ValvesOperation : SequenceOperation {
	uint8_t valve_shift;
	bool enable;

	ValvesOperation (uint8_t valve_shift, bool enable) : valve_shift(valve_shift), enable(enable) {}
	
	void apply () override {
		uint8_t command = (enable ? AV_NET_CMD_ON : AV_NET_CMD_OFF);

		next_op &= ~(0xFF << valve_shift); // remove last operation on that valve
		next_op |= command << valve_shift;
	}
};
struct WriteOperation : SequenceOperation {
	void show_valve (const char* name, uint8_t valve_shift) {
		uint8_t content = (next_op >> valve_shift) & 0xFF;
	
		if (content == 0) {
			Logger::log_eventf(Logger::DEBUG, " - %s: NO OPERATION", name);
		} else if (content == AV_NET_CMD_OFF) {
			Logger::log_eventf(Logger::DEBUG, " - %s: OFF", name);
		} else if (content == AV_NET_CMD_ON) {
			Logger::log_eventf(Logger::DEBUG, " - %s: ON", name);
		} else {
			Logger::log_eventf(Logger::ERROR, " - %s: UNKNOWN %hhx", content);
		}
	}
	void apply () override {
		Logger::log_eventf(Logger::DEBUG, "Sending valves operations :");
		show_valve("DN_NC", 16);
		show_valve("PX_NC", 8);
		show_valve("VX_NO", 0);

		dpr->write_valves(next_op);
		next_op = 0;
	}
};
struct ReadOperation : SequenceOperation {
	void apply () override {
		dpr->read_valves();
		
		Valves valves = Data::get_instance().get().valves;
	
		Logger::log_eventf(Logger::DEBUG, "Reading valves result :");
		Logger::log_eventf(Logger::DEBUG, " - vent_copv: %d", valves.valve_dpr_vent_copv);
		Logger::log_eventf(Logger::DEBUG, " - vent_fuel: %d", valves.valve_dpr_vent_fuel);
		Logger::log_eventf(Logger::DEBUG, " - vent_lox: %d",  valves.valve_dpr_vent_lox);
	}
};
struct DPRTestSequence {
private:
	std::vector<SequenceOperation*> operations;
public:
	void run () {
		for (auto op : operations) if (op != nullptr)
			op->apply();
	}
	DPRTestSequence (std::string file) {
		std::ifstream stream(file);
		
		while (!stream.fail()) {
			std::string buffer;
			stream >> buffer;
			if (stream.fail()) break ;

			if (buffer == "WRITE") operations.push_back(new WriteOperation());
			else if (buffer == "READ") operations.push_back(new ReadOperation());
			else if (buffer == "SLEEP") {
				int duration;
				stream >> duration;
				if (stream.fail()) {
					Logger::log_event(Logger::FATAL,
						"Wrong format encountered in the test sequence file after SLEEP (most likely missing duration in milliseconds).");
					exit(1);
				}
				if (duration < 0) {
					Logger::log_event(Logger::FATAL,
						"Cannot SLEEP a negative amount of time (might be due to missing duration or wrongly formatted int).");
					exit(1);
				}

				operations.push_back(new SleepOperation(duration));
			} else if (buffer == "DN_NC" || buffer == "PX_NC" || buffer == "VX_NO") {
				std::string op;
				stream >> op;

				if (stream.fail()) {
					Logger::log_eventf(Logger::FATAL,
						"Wrong format encountered in the test sequence file after valve %s (most likely missing status).", buffer.c_str());
					exit(1);
				}
				if (op != "ON" && op != "OFF") {
					Logger::log_eventf(Logger::FATAL,
						"Could not understand status %s in valve operation on %s (expeceted 'ON' or 'OFF')", op.c_str(), buffer.c_str());
					exit(1);
				}

				uint8_t valve_shift = 0;
				if (buffer == "PX_NC") valve_shift = 8;
				else if (buffer == "DN_NC") valve_shift = 16;

				operations.push_back(new ValvesOperation(valve_shift, op == "ON"));
			} else {
				Logger::log_eventf(Logger::FATAL,
						"Cannot understand keyword %s in test sequence.", buffer.c_str());
			}
		}
	}
};

int main() {
	Logger::init();
	/*
	I2CInterface::getInstance().open(AV_NET_ADDR_DPR_LOX);
	uint32_t timestamp(12345);
	std::cout << "Sending Timestamp 12345...";	
	I2CInterface::getInstance().write(AV_NET_ADDR_DPR_LOX, AV_NET_PRB_TIMESTAMP, (uint8_t*)&timestamp, AV_NET_XFER_SIZE); 
	std::cout << "Done\n";
	
	uint32_t test(0xFFFFFFFF);
	//I2CInterface::getInstance().write(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&test, AV_NET_XFER_SIZE);

	//uint32_t cmd(AV_NET_CMD_ON);
	//I2CInterface::getInstance().write(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_CLEAR_TO_IGNITE, (uint8_t*)&cmd, AV_NET_XFER_SIZE);


	for (int i(0); i < 10; ++i) {
		uint32_t valves(AV_NET_CMD_ON << 0 | AV_NET_CMD_ON << 8  |AV_NET_CMD_ON << 16);
		std::cout << "Writing valves...";
		I2CInterface::getInstance().write(AV_NET_ADDR_DPR_LOX, AV_NET_DPR_VALVES_STATE, (uint8_t*)&valves, AV_NET_XFER_SIZE);
		std::cout << "Done\n";
		AvTimer::sleep(100);
		valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8 | AV_NET_CMD_OFF << 16;
		std::cout << "Writing valves...";
		I2CInterface::getInstance().write(AV_NET_ADDR_DPR_LOX, AV_NET_DPR_VALVES_STATE, (uint8_t*)&valves, AV_NET_XFER_SIZE);
		std::cout << "Done\n";
		AvTimer::sleep(100);
	}
	*/

//	float value(0);
//	I2CInterface::getInstance().read(AV_NET_ADDR_DRP_LOX, AV_NET_PRB_T_EIN, (uint8_t*)&value, AV_NET_XFER_SIZE);	
//	std::cout << value << "\n";

	dpr = new DPR(AV_NET_ADDR_DPR_LOX);

	DPRTestSequence sequence(DPR_TEST_SEQUENCE);
	sequence.run();

	/*
	 *
	for (int i(0); i < 10; ++i) {
		std::cout << "Writing valves...";
		dpr.write_valves(valves);
		std::cout << "Done\n";
		AvTimer::sleep(100);
		valves = AV_NET_CMD_OFF << 0 | AV_NET_CMD_OFF << 8 | AV_NET_CMD_OFF << 16;
		std::cout << "Writing valves...";
		dpr.write_valves(valves);
		std::cout << "Done\n";
		AvTimer::sleep(100);
	}*/
}
