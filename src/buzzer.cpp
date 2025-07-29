#include "buzzer.h"
#include "config.h"
#include <pigpio.h>

static bool enabled = 0;

void Buzzer::enable() {
    enabled = 1;
    gpioWrite(BUZZER, enabled);
}

void Buzzer::disable() {
    enabled = 0;
    gpioWrite(BUZZER, enabled);
}

void Buzzer::toggle() {
    enabled = !enabled;
    gpioWrite(BUZZER, enabled);
}
