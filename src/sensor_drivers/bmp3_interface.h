#ifndef BMP3_INTERFACE_H
#define BMP3_INTERFACE_H
#include "bmp3_defs.h"
#include <cstdint>

class Bmp3_Interface {
public:
    virtual ~Bmp3_Interface() = default;
    virtual bmp3_status get_status() = 0;
    virtual bmp3_data get_sensor_data(uint8_t sensor_comp = BMP3_PRESS_TEMP) = 0;

};

#endif // BMP3_INTERFACE_H
