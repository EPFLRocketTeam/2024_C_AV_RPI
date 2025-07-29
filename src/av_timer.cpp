#include "av_timer.h"
#include <bits/chrono.h>
#include <chrono>

using namespace std::chrono;

static uint32_t start_ms(time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count());

uint32_t AvTimer::tick() {
    uint32_t now_ms(time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count());
    return now_ms - start_ms;
}
