#ifndef MOCK_BMI088_H
#define MOCK_BMI088_H

#include "IBmi08.h"
#include "bmi08x.h"
#include <random>

class MockBmi088 : public IBmi088 {
public:
    uint8_t get_accel_status() override { return 0xAB; }
    uint8_t get_gyro_status() override { return 0xCD; }

    bmi08_sensor_data_f get_accel_data() override {
        return {randf(), randf(), randf()};
    }

    bmi08_sensor_data_f get_gyro_data() override {
        return {randf(), randf(), randf()};
    }

    bmi08_sensor_data_f add_noise_to_data(bmi08_sensor_data_f data, float stddev) override {
        return {
            data.x + randf(stddev),
            data.y + randf(stddev),
            data.z + randf(stddev)
        };
    }

private:
    float randf(float stddev = 0.05f) {
        static std::default_random_engine gen;
        std::normal_distribution<float> dist(0.0f, stddev);
        return dist(gen);
    }
};

#endif
