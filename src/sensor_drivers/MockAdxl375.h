#ifndef MOCK_ADXL375_H
#define MOCK_ADXL375_H

#include "IAdxl375.h"
#include "adxl375.h" // for adxl375_data struct
#include <random>

class MockAdxl375 : public Adxl375
{
public:
    MockAdxl375(uint8_t addr = 0) : Adxl375(addr) {}
};

#endif // MOCK_ADXL375_H
