add_library(neopixel led_ws2812.cpp led_ws2812.h led_ws2812_defines.h)
target_include_directories(neopixel INTERFACE ${NEOPIXEL_DIR})
target_include_directories(neopixel PRIVATE ${SRC_DIR})
