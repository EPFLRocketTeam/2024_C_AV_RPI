// adxl375_mock.h
#ifndef ADXL375_MOCK_H
#define ADXL375_MOCK_H

#include "adxl375_interface.h"
#include <gmock/gmock.h>

class MockAdxl375 : public Adxl375_Interface {
public:
    MOCK_METHOD(int8_t, init, (uint8_t addr), (override));
    MOCK_METHOD(int8_t, deinit, (), (override));
    MOCK_METHOD(uint8_t, get_status, (), (override));
    MOCK_METHOD(adxl375_data, get_data, (), (override));
    MOCK_METHOD(void, set_offset, (const adxl375_data& offset), (override));
    MOCK_METHOD(void, set_scale, (float scale), (override));
    MOCK_METHOD(void, calibrate, (), (override));
};

#endif // ADXL375_MOCK_H