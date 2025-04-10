#pragma once

#include "../adxl375.h"
class MockAdxl375 : public Adxl375
{
public:
    MockAdxl375(uint8_t addr = 0) : Adxl375(addr) {}

    adxl375_data get_data() override
    {
        return Adxl375::get_data();
    }

    uint8_t get_status() override
    {
        return Adxl375::get_status();
    }

    adxl375_data add_noise_to_data(adxl375_data input, float stddev) override
    {
        return Adxl375::add_noise_to_data(input, stddev);
    }

    void calibrate() override
    {
        Adxl375::calibrate();
    }
};
