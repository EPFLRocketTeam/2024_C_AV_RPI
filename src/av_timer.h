#ifndef AV_TIMER_H
#define AV_TIMER_H

#include <cstdint>

namespace AvTimer {
    uint32_t tick();
    void sleep(const uint32_t duration_ms);
};

#endif /* AV_TIMER_H */
