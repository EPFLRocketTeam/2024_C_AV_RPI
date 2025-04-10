#ifndef MOCK_ADXL375_H
#define MOCK_ADXL375_H

#ifdef MOCK_SENSORS_ENABLED

#include "IAdxl375.h"
#include <random>

class Adxl375 : public IAdxl375 {
public:
    Adxl375(uint8_t addr) { (void)addr; }  // simulate constructor
    ~Adxl375() override = default;

    adxl375_data get_data() override {
        return {}; // dummy return
    }

    void calibrate() override {}
    std::string get_status() override { return "mock"; }

    static void add_noise_to_data(adxl375_data&, float) {}

private:
    float randf(float stddev) {
        static std::default_random_engine gen;
        std::normal_distribution<float> dist(0.0f, stddev);
        return dist(gen);
    }
};

#endif // MOCK_SENSORS_ENABLED
#endif // MOCK_ADXL375_H
