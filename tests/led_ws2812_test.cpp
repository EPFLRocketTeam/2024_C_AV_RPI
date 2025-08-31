#include "led_ws2812.h"

int main() {
    NeoPixel rgb_led(2);

    while (1) {
        rgb_led.effectsDemo();
    }
}
