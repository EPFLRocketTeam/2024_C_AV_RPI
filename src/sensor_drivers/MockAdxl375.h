// MockAdxl375.h
#ifndef MOCK_ADXL375_H
#define MOCK_ADXL375_H

#include <iostream>
#include <random>
#include "IAdxl375.h"   
#include "adxl375.h" 

class MockAdxl375 : public IAdxl375 {
public:
    adxl375_data get_data() override {
        return {0.0f + rand_noise(), rand_noise(), 0.2f + rand_noise()};
    }

    uint8_t get_status() override {
        return 0xFF; // dummy OK status
    }

    void calibrate() override {
        std::cout << "[MockAdxl375] calibrate() called\n";
    }

    adxl375_data add_noise_to_data(const adxl375_data& input, float stddev) override {
        return {
            input.x + rand_noise(stddev),
            input.y + rand_noise(stddev),
            input.z + rand_noise(stddev)
        };
    }

private:
    float rand_noise(float stddev = 0.05f) {
        static std::default_random_engine gen;
        std::normal_distribution<float> dist(0.0f, stddev);
        return dist(gen);
    }
};

#endif // MOCK_ADXL375_H
