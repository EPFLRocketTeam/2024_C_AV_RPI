// adxl375_interface.h
#ifndef ADXL375_INTERFACE_H
#define ADXL375_INTERFACE_H

#include <cstdint>

struct adxl375_data {
	float x;
	float y;
	float z;
};

class Adxl375_Interface {
public:
    virtual ~Adxl375_Interface() noexcept = default;
    virtual uint8_t get_status() = 0;
    virtual adxl375_data get_data() = 0;
    virtual void set_offset(adxl375_data offset) = 0;
    virtual void set_scale(float scale) = 0;
    virtual void calibrate() = 0;
};

#endif // ADXL375_INTERFACE_H