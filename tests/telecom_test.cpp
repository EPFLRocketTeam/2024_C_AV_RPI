#include <pigpio.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include "PacketDefinition_Firehorn.h"
#include "av_state.h"
#include "config.h"
#include "data.h"
#include "telecom.h"
#include "av_timer.h"


void test_downlink(Telecom& ttc);
void test_uplink(Telecom& ttc);

int main() {
    std::cout << "\x1b[7mAvionics Telecom Tests\x1b[0m\n\n";
    gpioInitialise();

    Telecom ttc;
    std::cout << "Initializing Lora modules... ";
    ttc.begin();
    std::cout << "\x1b[32mOK\x1b[0m\n";

    std::cout << "Testing downlink (packet transmission)\n";
//    test_downlink(ttc);
    std::cout << "Testing uplink (packet reception)\n";
 //   test_uplink(ttc);
    unsigned timestamp(341400);
    Data& goat(Data::get_instance());
    while (1) {
        std::cout << " - Writing " << timestamp << " as AV_TIMESTAMP to GOAT... ";
        goat.write(Data::AV_TIMESTAMP, &timestamp);
	State state(State::ARMED);
	goat.write(Data::AV_STATE, &state);
	float lpb(5);
	goat.write(Data::BAT_LPB_VOLTAGE, &lpb);
	float hpb(24);
	goat.write(Data::BAT_HPB_VOLTAGE, &hpb);
	double lon(112.34);
	goat.write(Data::NAV_GNSS_POS_LNG, &lon);
	double lat(113.34);
	goat.write(Data::NAV_GNSS_POS_LAT, &lat);
	double alt(2974.342);
	goat.write(Data::NAV_GNSS_POS_ALT, &alt);
	double N2_pres(150.42);
	goat.write(Data::PR_SENSOR_P_NCO, &N2_pres);
	double Fuel_pres(145.423);
	goat.write(Data::PR_SENSOR_P_ETA, &Fuel_pres);
	double lox_pres(132.564);
	goat.write(Data::PR_SENSOR_P_OTA, &lox_pres);
	double fuel_level(23.56);
	goat.write(Data::PR_SENSOR_L_ETA, &fuel_level);
	double lox_level(12.34);
	goat.write(Data::PR_SENSOR_L_OTA, &lox_level);
	double N2_temp(-180);
	goat.write(Data::PR_SENSOR_T_NCO, &N2_temp);
	double lox_temp(90);
	goat.write(Data::PR_SENSOR_T_OTA, &lox_temp);
	float temp(60);
	goat.write(Data::AV_FC_TEMPERATURE, &temp);
        std::cout << "Done\n";
        //std::cout << " - Writing " << i << " as AV_STATE to GOAT... ";
        //goat.write(Data::AV_STATE, (State*)&i);
//        std::cout << "Done\n";


        std::cout << " - Sending packet on downlink freq (" << AV_DOWNLINK_FREQUENCY << ")... ";
        ttc.send_telemetry();
        std::cout << "\x1b[32mOK\x1b[0m\n";

	ttc.update();

	AvTimer::sleep(1e3);
    }
    gpioTerminate();
}


void test_uplink(Telecom& ttc) {
    auto start(std::chrono::steady_clock::now());
    auto now(std::chrono::steady_clock::now());
    std::cout << " - Listening on uplink freq (" << UPLINK_FREQUENCY << ") for 10s...\n";
    while (1/*std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() < 10e3*/) {
        ttc.update();
        //usleep(1e4);
        //now = std::chrono::steady_clock::now();
    }
    std::cout << "Done\n";
}

void test_downlink(Telecom& ttc) {
    unsigned timestamp(341400);
    Data& goat(Data::get_instance());
    for (;;) {
        //std::cout << "[" << i << "]--------------------\n";

        ++timestamp;
        std::cout << " - Writing " << timestamp << " as AV_TIMESTAMP to GOAT... ";
        goat.write(Data::AV_TIMESTAMP, &timestamp);
	State state(State::ARMED);
	goat.write(Data::AV_STATE, &state);
	int temp(1234);
	goat.write(Data::AV_FC_TEMPERATURE, &temp);
        std::cout << "Done\n";
        //std::cout << " - Writing " << i << " as AV_STATE to GOAT... ";
        //goat.write(Data::AV_STATE, (State*)&i);
        std::cout << "Done\n";


        std::cout << " - Sending packet on downlink freq (" << AV_DOWNLINK_FREQUENCY << ")... ";
        ttc.send_telemetry();
        std::cout << "\x1b[32mOK\x1b[0m\n";
    }
}

