#include <iostream>
#include <thread>
#include <pigpio.h>
#include "telecom.h"

#define UPLINK_TEST
// #define DOWNLINK_TEST

Telecom tel;

void poll_uplink();
void send_downlink();

int main() {
    gpioInitialise();

    const bool success(tel.begin());
    if (!success) {
        return success;
    }

#ifdef UPLINK_TEST
    poll_uplink();
#elif defined DOWNLINK_TEST
    send_downlink();
#endif

    std::cout << "Program exited normally\n";

    gpioTerminate();
}

void poll_uplink() {
    unsigned count(100);
    while (--count) {
        tel.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10 Hz
    }
}

void send_downlink() {
    unsigned count(0);
    av_downlink_t packet;

    while (count < 1000) {
        packet.packet_nbr = count;
        packet.timestamp = gpioTick() / 1000;

        packet.gnss_lat = 32;
        packet.gnss_lon = 32;
        packet.gnss_alt = 32;
        
        // We don't use a 2nd GPS
        packet.gnss_lat_r = 33;
        packet.gnss_lon_r = 33;
        packet.gnss_alt_r = 33;

        packet.gnss_vertical_speed = 0;

        packet.N2_pressure = 64;
        packet.fuel_pressure = 64;
        packet.LOX_pressure = 64;
        packet.fuel_level = 64;
        packet.LOX_level = 64;
        packet.engine_temp = 64;
        packet.igniter_pressure = 64;
        packet.LOX_inj_pressure = 64;
        packet.fuel_inj_pressure = 64;
        packet.chamber_pressure = 64;

        packet.engine_state = {0, 0, 0, 0, 0, 0};
        packet.av_state = (uint8_t)FLIGHTMODE::INIT;
        packet.gnss_choice = 0;

        tel.send_packet(CAPSULE_ID::AV_TELEMETRY, (uint8_t*)&packet, av_downlink_size);
    
        ++count;

        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // 10 Hz req, 2s for debug
    }

}