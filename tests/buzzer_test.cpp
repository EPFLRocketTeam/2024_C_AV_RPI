#include <iostream>
#include <pigpio.h>
#include <unistd.h>
#include "config.h"

int main() {
    gpioInitialise();

    gpioWrite(LED_LORA_RX, 1);
    gpioWrite(LED_LORA_TX, 1);

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

    usleep(3e6);
    gpioWrite(LED_LORA_RX, 0);
    gpioWrite(LED_LORA_TX, 0);

    gpioTerminate();
}

