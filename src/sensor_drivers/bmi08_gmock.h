#include <gmock/gmock.h>
#include "bmi08_interface.h" // Ensure this includes the forward declarations of structs like bmi08_dev, etc.
#include <stdint.h>
class MockBMI08 : public Bmi088_Interface {
public:
    MockBMI08(uint8_t accel_addr, uint8_t gyro_addr) : accel_addr(accel_addr), gyro_addr(gyro_addr) {};
    MOCK_METHOD(uint8_t, get_accel_status, (), (override));
    MOCK_METHOD(uint8_t, get_gyro_status, (), (override));
    MOCK_METHOD(bmi08_sensor_data_f, get_accel_data, (), (override));
    MOCK_METHOD(bmi08_sensor_data_f, get_gyro_data, (), (override));
private:
    uint8_t accel_addr;
    uint8_t gyro_addr;
};
