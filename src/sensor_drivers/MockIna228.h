#ifndef MOCK_INA228_H
#define MOCK_INA228_H


#include "IIna228.h"
#include <random>

class MockIna228 : public IIna228 {
public:
    float getBusVoltage() override {
        return 12.0 + randf(0.1);  // Simulated 12V system
    }

    int setMaxCurrentShunt(float, float) override {
        return 0;  // Always success
    }

private:
    float randf(float stddev) {
        static std::default_random_engine gen;
        std::normal_distribution<float> dist(0.0, stddev);
        return dist(gen);
    }
};

#endif // MOCK_INA228_H
