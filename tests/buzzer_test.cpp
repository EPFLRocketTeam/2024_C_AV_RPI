#include <iostream>
#include <pigpio.h>
#include <unistd.h>
#include "config.h"

int main() {
    gpioInitialise();

    gpioWrite(BUZZER, 1);
    usleep(100e3);
    gpioWrite(BUZZER, 0);
    usleep(100e3);
    gpioWrite(BUZZER, 1);
    usleep(100e3);
    gpioWrite(BUZZER, 0);
    usleep(100e3);
    gpioWrite(BUZZER, 1);
    usleep(100e3);
    gpioWrite(BUZZER, 0);

    gpioTerminate();
}

