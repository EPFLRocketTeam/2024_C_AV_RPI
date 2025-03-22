#include <bmp3_defs.h>
#include <bmi08_defs.h>
#include <adxl375.h>
#include <gmock/gmock.h>

class MockNavSensors
{
public:
    MOCK_METHOD(adxl375_data, getAdxl, (), (const));
    MOCK_METHOD(adxl375_data, getAdxlAux, (), (const));
    MOCK_METHOD(bmi08_sensor_data_f, getBmiAccel, (), (const));
    MOCK_METHOD(bmi08_sensor_data_f, getBmiGyro, (), (const));
    MOCK_METHOD(bmi08_sensor_data_f, getBmiAuxAccel, (), (const));
    MOCK_METHOD(bmi08_sensor_data_f, getBmiAuxGyro, (), (const));
    MOCK_METHOD(bmp3_data, getBmp, (), (const));
    MOCK_METHOD(bmp3_data, getBmpAux, (), (const));
};


TEST(NavSensorsTest, MockExample)
{
    MockNavSensors mockSensor;

    adxl375_data fakeData = {1.0, 2.0, 3.0};

    EXPECT_CALL(mockSensor, getAdxl()).WillOnce(testing::Return(fakeData));

    auto result = mockSensor.getAdxl(); 
    ASSERT_EQ(result.x, 1.0);
    ASSERT_EQ(result.y, 2.0);
    ASSERT_EQ(result.z, 3.0);
}

