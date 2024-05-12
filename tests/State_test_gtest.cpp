#include "gtest/gtest.h"
#include "gmock/gmock.h"

class DataGenerator {
public:
    SensFiltered generateData(double flightTime) {
        SensFiltered data;
        // Customize data generation logic based on flight time or phase
        // Example: adjust sensor values based on a sine wave or a simple model
        data.value = sin(flightTime) * 100;  // Example model
        return data;
    }
};

TEST(DataTests, HighFrequencyUpdateSimulation) {
    MockTelecom mockTelecom;
    MockSensors mockSensors;
    DataGenerator generator;
    double simulatedTime = 0.0;  // Simulated flight time in seconds

    EXPECT_CALL(mockSensors, getFilteredData())
        .WillRepeatedly([&generator, &simulatedTime]() {
            return generator.generateData(simulatedTime);
        });

    EXPECT_CALL(mockTelecom, update())
        .WillRepeatedly([&generator, &simulatedTime]() {
            return generator.generateTelecom(simulatedTime);
        });

    Data data(std::make_unique<MockTelecom>(), std::make_unique<MockSensors>());

    // Simulation loop (e.g., simulate 10 seconds at 100Hz)
    for (int i = 0; i < 1000; ++i) {
        data.update();
        simulatedTime += 0.01;  // Advance time by 0.01 seconds (10ms)
        DataDump dump = data.dump();
        // Optionally, add assertions or logging to inspect output
    }
}