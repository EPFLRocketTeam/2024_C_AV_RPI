#pragma once

#include "IAdxl375.h"
#include "adxl375.h"
#include <random>

class MockAdxl375 : public IAdxl375
{
public:
    MockAdxl375(uint8_t addr = 0) : address(addr) {}

    adxl375_data get_data() override
    {
        return adxl375_data{1.0f, 2.0f, 3.0f};
    }

    uint8_t get_status() override
    {
        return 0x42; 
    }

    adxl375_data add_noise_to_data(adxl375_data input, float stddev) override
    {
        std::default_random_engine generator;
        std::normal_distribution<float> distribution(0.0, stddev);
        input.x += distribution(generator);
        input.y += distribution(generator);
        input.z += distribution(generator);
        return input;
    }

    void calibrate() override
    {
        
    }

private:
    uint8_t address;
};
