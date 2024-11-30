#include <gmock/gmock.h>
#include "bmp3_interface.h"

class MockBMP3 : public Bmp3_Interface {
public:
    MockBMP3(u_int8_t addr) : addr(addr) {};
    MOCK_METHOD(bmp3_status, get_status, (), (override));
    MOCK_METHOD(bmp3_data, get_sensor_data, (uint8_t sensor_comp), (override));
private:
    u_int8_t addr;
};
