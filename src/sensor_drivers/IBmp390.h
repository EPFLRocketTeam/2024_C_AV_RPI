#ifndef IBMP390_H
#define IBMP390_H

#include "bmp3.h"  // for bmp3_status and bmp3_data

class IBmp390 {
public:
    virtual ~IBmp390() = default;

    virtual bmp3_status get_status() = 0;
    virtual bmp3_data get_sensor_data(uint8_t sensor_comp = BMP3_PRESS_TEMP) = 0;
    virtual bmp3_data add_noise_to_data(bmp3_data original_value, double stddev) = 0;
};

#endif
