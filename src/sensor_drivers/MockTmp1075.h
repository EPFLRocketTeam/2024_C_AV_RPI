#ifndef MOCK_TMP1075_H
#define MOCK_TMP1075_H

#include "ITmp1075.h"
#include <random>

class MockTmp1075 : public ITmp1075
{
public:
    MockTmp1075(int) {}

    float getTemperatureCelsius() override
    {
        return add_noise_to_data(25.0f, 1.0f); // base + noise
    }

    float add_noise_to_data(float value, float stddev)
    {
        return value + randf(stddev);
    }

private:
    float randf(float stddev)
    {
        static std::default_random_engine gen(std::random_device{}());
        std::normal_distribution<float> dist(0.0f, stddev);
        return dist(gen);
    }
};

#endif // MOCK_TMP1075_H
