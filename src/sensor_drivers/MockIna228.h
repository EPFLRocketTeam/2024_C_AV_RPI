#ifndef MOCK_INA228_H
#define MOCK_INA228_H

#include "IIna228.h"
#include <random>

class MockIna228 : public IIna228
{
public:
    MockIna228(int, double, double) {}

    float getBusVoltage() override
    {
        return add_noise_to_data(12.0f, 0.1f); // base + noise
    }

    float add_noise_to_data(float value, float stddev)
    {
        return value + randf(stddev);
    }

    int setMaxCurrentShunt(float, float) override
    {
        return 0;
    }

private:
    float randf(float stddev)
    {
        static std::default_random_engine gen(std::random_device{}());
        std::normal_distribution<float> dist(0.0f, stddev);
        return dist(gen);
    }
};

class INA228Exception : public std::exception
{
public:
    const char *what() const noexcept override
    {
        return "Mock INA228 exception";
    }
};

#endif // MOCK_INA228_H
