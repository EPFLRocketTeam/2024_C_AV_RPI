#ifndef BMI08_INTERFACE_H
#define BMI08_INTERFACE_H
#include <stdint.h>
#include "bmi08_defs.h"
class Bmi088_Interface {
    public:
    virtual ~Bmi088_Interface() = default;
    virtual uint8_t get_accel_status() = 0;
    virtual uint8_t get_gyro_status() = 0;
    virtual bmi08_sensor_data_f get_accel_data() = 0;
    virtual bmi08_sensor_data_f get_gyro_data() = 0;
};
#endif //BMI08_INTERFACE_H