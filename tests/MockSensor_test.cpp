#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <unistd.h>
#include "MockSensor.cpp"

using ::testing::Return;
using ::testing::Sequence;

class MockSensorsTest : public ::testing::Test {
protected:
    MockSensors mock_sensors;

    void SetUp() override {


    }

    void TearDown() override {
    }
};

void expectUpdateStatus(MockSensors& mock_sensors) {
    EXPECT_CALL(mock_sensors.adxl1, get_status()).Times(1).WillOnce(Return(0x10));
    EXPECT_CALL(mock_sensors.adxl2, get_status()).Times(1);
    EXPECT_CALL(mock_sensors.bmi1, get_accel_status()).Times(1).WillOnce(Return(0x30));
    EXPECT_CALL(mock_sensors.bmi1, get_gyro_status()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(mock_sensors.bmi2, get_accel_status()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(mock_sensors.bmi2, get_gyro_status()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(mock_sensors.bmp1, get_status()).Times(1);
    EXPECT_CALL(mock_sensors.bmp2, get_status()).Times(1);
}

TEST_F(MockSensorsTest, CalibrateTest) {
    // Expect calls to mock components' calibrate methods
    EXPECT_CALL(mock_sensors.adxl1, calibrate()).Times(1);
    EXPECT_CALL(mock_sensors.adxl2, calibrate()).Times(1);

    // Call the method under test
    mock_sensors.calibrate();
}

TEST_F(MockSensorsTest, UpdateStatus) {
    // Expect calls for ADXL sensors
    ON_CALL(mock_sensors.adxl1, get_status()).WillByDefault(Return(0x10));

    EXPECT_CALL(mock_sensors.adxl1, get_status()).Times(1).WillOnce(Return(0x10));
    EXPECT_CALL(mock_sensors.adxl2, get_status()).Times(1);

    // Expect calls for BMI sensors
    ON_CALL(mock_sensors.bmi1, get_accel_status()).WillByDefault(Return(0x30));
    EXPECT_CALL(mock_sensors.bmi1, get_accel_status()).Times(1).WillOnce(Return(0x30));
    EXPECT_CALL(mock_sensors.bmi1, get_gyro_status()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(mock_sensors.bmi2, get_accel_status()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(mock_sensors.bmi2, get_gyro_status()).Times(1).WillOnce(Return(0));

    // Expect calls for BMP sensors
    bmp3_status sensor_status = {0};

    // Simulate setting sensor status flags
    sensor_status.sensor.cmd_rdy = 1;    // Command ready status
    sensor_status.sensor.drdy_press = 1; // Pressure data ready
    sensor_status.sensor.drdy_temp = 0;  // Temperature data not ready

    // Simulate setting error flags
    sensor_status.err.fatal = 0;         // No fatal error
    sensor_status.err.cmd = 0;           // No command error
    sensor_status.err.conf = 1;          // Configuration error

    // Simulate interrupt status
    sensor_status.intr.fifo_wm = 0;      // FIFO watermark not reached
    sensor_status.intr.fifo_full = 0;    // FIFO not full
    sensor_status.intr.drdy = 1;         // Data ready interrupt

    // Simulate power-on reset status
    sensor_status.pwr_on_rst = 1;        // Power-on reset occurred

    ON_CALL(mock_sensors.bmp1, get_status()).WillByDefault(Return(sensor_status));
    EXPECT_CALL(mock_sensors.bmp1, get_status()).Times(1);
    EXPECT_CALL(mock_sensors.bmp2, get_status()).Times(1);

    // Call the method under test
    mock_sensors.update_status();
    // read the data from the GOAT

    auto data = Data::get_instance().get();
    // print the data
    EXPECT_EQ(data.stat.adxl_status, 0x10);
    EXPECT_EQ(data.stat.bmi_accel_status, 0x30);
    EXPECT_EQ(data.stat.bmp_status, sensor_status);
    EXPECT_NE(data.stat.bmp_aux_status, sensor_status);



}


TEST_F(MockSensorsTest, Update){

    expectUpdateStatus(mock_sensors);
    EXPECT_CALL(mock_sensors.adxl1, get_data()).Times(1);
    EXPECT_CALL(mock_sensors.adxl2, get_data()).Times(1);
    EXPECT_CALL(mock_sensors.bmi1, get_accel_data()).Times(1);
    EXPECT_CALL(mock_sensors.bmi1, get_gyro_data()).Times(1);
    EXPECT_CALL(mock_sensors.bmi2, get_accel_data()).Times(1);
    EXPECT_CALL(mock_sensors.bmi2, get_gyro_data()).Times(1);
    EXPECT_CALL(mock_sensors.bmp1, get_sensor_data(BMP3_PRESS_TEMP)).Times(1);
    EXPECT_CALL(mock_sensors.bmp2, get_sensor_data(BMP3_PRESS_TEMP)).Times(1);

    ON_CALL(mock_sensors.i2cgps, available()).WillByDefault(Return(0));
    EXPECT_CALL(mock_sensors.i2cgps, available()).Times(1);
        


     mock_sensors.update();
}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


// Compare bmp3_status
inline bool operator==(const bmp3_status& lhs, const bmp3_status& rhs) {
    return lhs.intr.fifo_wm == rhs.intr.fifo_wm &&
           lhs.intr.fifo_full == rhs.intr.fifo_full &&
           lhs.intr.drdy == rhs.intr.drdy &&
           lhs.sensor.cmd_rdy == rhs.sensor.cmd_rdy &&
           lhs.sensor.drdy_press == rhs.sensor.drdy_press &&
           lhs.sensor.drdy_temp == rhs.sensor.drdy_temp &&
           lhs.err.fatal == rhs.err.fatal &&
           lhs.err.cmd == rhs.err.cmd &&
           lhs.err.conf == rhs.err.conf &&
           lhs.pwr_on_rst == rhs.pwr_on_rst;
}

// define !=
inline bool operator!=(const bmp3_status& lhs, const bmp3_status& rhs) {
    return !(lhs == rhs);
}