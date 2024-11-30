#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <unistd.h>
#include "adxl375_gmock.h" // Include your mock class

#define ITERATION 3

// Test fixture for ADXL375 tests
class Adxl375Test : public ::testing::Test {
protected:
    MockAdxl375 adxl1{0x53}; 
    MockAdxl375 adxl2{0x53}; 
    adxl375_data mockData;

    void SetUp() override {
        mockData = {0, 0, 9.81}; 
    }
};

TEST_F(Adxl375Test, DataRetrievalTest) {
    int loop = 0;

    EXPECT_CALL(adxl1, get_status())
        .Times(ITERATION)
        .WillRepeatedly(::testing::Return(1 << 7)); 

    EXPECT_CALL(adxl1, get_data())
        .Times(ITERATION)
        .WillRepeatedly(::testing::Return(mockData)); 

    EXPECT_CALL(adxl2, get_status())
        .Times(ITERATION)
        .WillRepeatedly(::testing::Return(1 << 7)); 

    EXPECT_CALL(adxl2, get_data())
        .Times(ITERATION)
        .WillRepeatedly(::testing::Return(mockData)); 
    EXPECT_CALL(adxl1, calibrate()).Times(1);
    EXPECT_CALL(adxl2, calibrate()).Times(1);

    adxl1.calibrate();
    adxl2.calibrate();
    while (loop < ITERATION) {
        if (adxl1.get_status() & (1 << 7)) {
            adxl375_data data = adxl1.get_data();
            std::cout << "Primary data[" << loop << "] x: " << data.x << ", y: "
                      << data.y << ", z: " << data.z << std::endl;

            // Validate mock data
            ASSERT_GT(data.x, -5.89);
            ASSERT_LT(data.x, 13.73);
            ASSERT_GT(data.y, -13.73);
            ASSERT_LT(data.y, 5.89);
            ASSERT_GT(data.z, -11.77);
            ASSERT_LT(data.z, 23.5);
        }

        if (adxl2.get_status() & (1 << 7)) {
            adxl375_data data = adxl2.get_data();
            std::cout << "Secondary data[" << loop << "] x: " << data.x << ", y: "
                      << data.y << ", z: " << data.z << std::endl;

            ASSERT_GT(data.x, -5.89);
            ASSERT_LT(data.x, 13.73);
            ASSERT_GT(data.y, -13.73);
            ASSERT_LT(data.y, 5.89);
            ASSERT_GT(data.z, -11.77);
            ASSERT_LT(data.z, 23.5);
        }

        usleep(100); 
        loop++;
    }

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}