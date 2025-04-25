#include <pigpio.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include "PacketDefinition_Firehorn.h"
#include "av_state.h"
#include "config.h"
#include "data.h"
#include "telecom.h"


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
    test_downlink(ttc);
    std::cout << "Testing uplink (packet reception)\n";
    test_uplink(ttc);

    gpioTerminate();
}


void test_uplink(Telecom& ttc) {
    auto start(std::chrono::steady_clock::now());
    auto now(std::chrono::steady_clock::now());
    std::cout << " - Listening on uplink freq (" << UPLINK_FREQUENCY << ") for 10s...\n";
    while (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() < 10e3) {
        ttc.update();
        usleep(1e4);
        now = std::chrono::steady_clock::now();
    }
    std::cout << "Done\n";
}

void test_downlink(Telecom& ttc) {
    unsigned timestamp(time(0));
    Data& goat(Data::get_instance());
    for (unsigned i(0); i < 10; ++i) {
        std::cout << "[" << i << "]--------------------\n";

        timestamp = time(0);
        std::cout << " - Writing " << timestamp << " as AV_TIMESTAMP to GOAT... ";
        goat.write(Data::AV_TIMESTAMP, &timestamp);
        std::cout << "Done\n";
        std::cout << " - Writing " << i << " as AV_STATE to GOAT... ";
        goat.write(Data::AV_STATE, (State*)&i);
        std::cout << "Done\n";


        std::cout << " - Sending packet on downlink freq (" << AV_DOWNLINK_FREQUENCY << ")... ";
        ttc.send_telemetry();
        std::cout << "\x1b[32mOK\x1b[0m\n";
    }
}

