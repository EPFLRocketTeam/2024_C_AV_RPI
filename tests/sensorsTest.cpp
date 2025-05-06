#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Sensors.h"
#include "data.h"
#include "config.h"

using ::testing::_;
using ::testing::Return;

// Optional: Mock class for Data
class MockData : public Data {
public:
    MOCK_METHOD(void, write, (uint32_t type, const void* payload), (override));
    static MockData* InitMock() {
        static MockData instance;
        instance_override = &instance;
        return &instance;
    }
    static void ClearMock() {
        instance_override = nullptr;
    }
protected:
    MockData() : Data() {}
};

// Fixture for Sensors tests
class SensorsTest : public ::testing::Test {
protected:
    Sensors* sensors;

    void SetUp() override {
        // Replace Data singleton with mock
        MockData::InitMock();
        sensors = new Sensors();
    }

    void TearDown() override {
        delete sensors;
        MockData::ClearMock();
    }
};

// Example test: Calibration writes the calibrated flag
TEST_F(SensorsTest, CalibrationWritesFlag) {
    MockData* mockData = MockData::InitMock();
    bool expected = true;
    EXPECT_CALL(*mockData, write(Data::EVENT_CALIBRATED, &expected))
        .Times(1);

    sensors->calibrate();
}

// Example test: update returns true in simulation mode
TEST_F(SensorsTest, UpdateReturnsTrueInSimulation) {
    sensors->simulation_mode = true;
    ASSERT_TRUE(sensors->update());
}

// Example test: GNSS update (assumes simulation mode)
TEST_F(SensorsTest, GNSSUpdateWritesValues) {
    MockData* mockData = MockData::InitMock();

    // Simulate GPS values (if gps is public or mockable)
    sensors->gps.time.setTime(12, 34, 56, 78);
    sensors->gps.time.commit();
    sensors->gps.date.setDate(2025, 4, 25);
    sensors->gps.date.commit();
    sensors->gps.location.setLatitude(46.5191);
    sensors->gps.location.setLongitude(6.5668);
    sensors->gps.location.commit();
    sensors->gps.altitude.set(1200.0);
    sensors->gps.altitude.commit();
    sensors->gps.course.set(90.0);
    sensors->gps.course.commit();

    EXPECT_CALL(*mockData, write(Data::NAV_GNSS_TIME_YEAR, _)).Times(1);
    EXPECT_CALL(*mockData, write(Data::NAV_GNSS_POS_LAT, _)).Times(1);
    EXPECT_CALL(*mockData, write(Data::NAV_GNSS_COURSE, _)).Times(1);

    sensors->update(); // should trigger GPS parsing
}

