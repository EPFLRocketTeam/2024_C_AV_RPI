#include <iostream>
#include "intranet_commands.h"
#include "i2c_interface.h"
#include "av_timer.h"
#include "logger.h"
#include "PR_board.h"

/* ==================================
            CONSTANTS & CONFIG
   ==================================*/

#define RATE_START_MS 200   // 5Hz
#define RATE_STOP_MS 10     // 100Hz
#define RATE_STEP_MS 10
#define STEP_DURATION_MS 5e3

/* ==================================
                TEST CODE
   ==================================*/

int main() {
    Logger::init();
    Logger::log_event(Logger::DEBUG, "Staring PRB Stauration Test\n");
    PR_board h_prb;

    uint32_t s_rate_ms = RATE_START_MS;
    uint32_t s_packet_to_send = (uint32_t) STEP_DURATION_MS/s_rate_ms;
    uint32_t s_packet_sent = 0;
    uint32_t s_packet_lost = 0;

    h_prb.send_wake_up();
    AvTimer::sleep(1000); // Simple delay for logs observation

    while(s_rate_ms >= RATE_STOP_MS){
        if(s_packet_sent++ >= s_packet_to_send) {
            Logger::log_eventf("Packet loss rate @ %d [ms] delta - %d/%d\n", s_rate_ms, s_packet_lost, s_packet_to_send);
            s_rate_ms -= RATE_STEP_MS;
            s_packet_to_send = (uint32_t) STEP_DURATION_MS/s_rate_ms;
            s_packet_sent = 0;
        }
        
        try {
            h_prb.read_combustion_chamber();
        } catch(PRBoardException e) {
            s_packet_lost++;
        }

        AvTimer::sleep(s_rate_ms);
    }

    Logger::log_event(Logger::DEBUG, "Staring PRB Stauration Test\n");
    Logger::terminate();
}