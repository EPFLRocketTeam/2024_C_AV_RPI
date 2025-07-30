#include "av_timer.h"
#include <chrono>
#include <thread>

using namespace std::chrono;

static uint32_t start_ms(time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count());

uint32_t AvTimer::tick() {
    uint32_t now_ms(time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count());
    return now_ms - start_ms;
}

void AvTimer::sleep(const uint32_t duration_ms) {
    std::this_thread::sleep_for(milliseconds(duration_ms));
}

