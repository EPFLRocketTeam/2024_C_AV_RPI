#ifndef MOCK_TMP1075_H
#define MOCK_TMP1075_H

#include "ITmp1075.h"
#include <random>

class MockTmp1075 : public ITmp1075 {
public:
    float getTemperatureCelsius() override {
        return 25.0 + randf(1.0);  // Around room temp
    }

private:
    float randf(float stddev) {
        static std::default_random_engine gen;
        std::normal_distribution<float> dist(0.0f, stddev);
        return dist(gen);
    }
};

#endif
