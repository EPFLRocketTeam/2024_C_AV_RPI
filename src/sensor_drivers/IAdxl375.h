// IAdxl375.h
#ifndef IADXL375_H
#define IADXL375_H

class IAdxl375 {
public:
    virtual ~IAdxl375() = default;

    virtual adxl375_data get_data() = 0;
    virtual uint8_t get_status() = 0;
    virtual void calibrate() = 0;
    virtual adxl375_data add_noise_to_data(const adxl375_data& input, float stddev) = 0;
};

#endif // IADXL375_H
