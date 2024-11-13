#include <gmock/gmock.h>
#include "bmi08_interface.h" // Ensure this includes the forward declarations of structs like bmi08_dev, etc.

class MockBMI08 : public Bmi088_Interface {
public:
    MOCK_METHOD(uint8_t, get_accel_status, (), (override));
    MOCK_METHOD(uint8_t, get_gyro_status, (), (override));
    MOCK_METHOD(bmi08_sensor_data_f, get_accel_data, (), (override));
    MOCK_METHOD(bmi08_sensor_data_f, get_gyro_data, (), (override));
};
