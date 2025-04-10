#ifndef IBMI088_H
#define IBMI088_H

#include <stdint.h>

// ✅ Forward declaration
struct bmi08_sensor_data_f;

class IBmi088 {
public:
    virtual ~IBmi088() = default;
    virtual uint8_t get_accel_status() = 0;
    virtual uint8_t get_gyro_status() = 0;
    virtual bmi08_sensor_data_f get_accel_data() = 0;
    virtual bmi08_sensor_data_f get_gyro_data() = 0;
    virtual bmi08_sensor_data_f add_noise_to_data(bmi08_sensor_data_f data, float noise_level) = 0;
};

#endif
