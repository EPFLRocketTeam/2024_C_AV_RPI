#ifndef MOCK_ADXL375_H
#define MOCK_ADXL375_H

#ifdef MOCK_SENSORS_ENABLED

#include "IAdxl375.h"
#include "adxl375.h" // for adxl375_data struct
#include <random>

class Adxl375 : public IAdxl375 {
public:
    Adxl375(uint8_t addr) { (void)addr; }
    ~Adxl375() override = default;

    adxl375_data get_data() override {
        return {}; // return dummy struct
    }

    void calibrate() override {}

    uint8_t get_status() override { return 0x00; } // pretend it's OK

    static void add_noise_to_data(adxl375_data&, float) {}
};

#endif // MOCK_SENSORS_ENABLED
#endif // MOCK_ADXL375_H
