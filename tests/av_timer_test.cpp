#include <iostream>
#include "av_timer.h"

int main() {
    uint32_t now(0);
    uint32_t old(0);
    uint32_t delta(0);
    for (int i(1); i < 11; ++i) {
        old = now;
        now = AvTimer::tick();
        delta = now - old;

        std::cout << "Timestamp: " << now << " ms.\n"
                  << "Sleep time: " << delta << " ms.\n\n";

        AvTimer::sleep(i * 100);
    }
}
