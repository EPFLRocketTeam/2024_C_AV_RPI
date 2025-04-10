#ifndef MOCK_BMP390_H
#define MOCK_BMP390_H

#include "IBmp390.h"
#include "bmp3.h" 
#include "bmp3_defs.h"

#include <random>

class MockBmp390 : public IBmp390 {
public:
    bmp3_status get_status() override {
        bmp3_status status{};
        status.pwr_on_rst = 1;
        return status;
    }

    bmp3_data get_sensor_data(uint8_t = BMP3_PRESS_TEMP) override {
        return {101325.0 + randf(100), 25.0 + randf(1)}; // pressure in Pa, temp in C
    }

    bmp3_data add_noise_to_data(bmp3_data original, double stddev) override {
        return {
            original.pressure + randf(stddev * 100),
            original.temperature + randf(stddev)
        };
    }

private:
    double randf(double stddev) {
        static std::default_random_engine gen;
        std::normal_distribution<double> dist(0.0, stddev);
        return dist(gen);
    }
};

#endif
