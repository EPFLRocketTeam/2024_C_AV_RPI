add_executable(adxl375_test ADXL375_test.cpp)
target_link_libraries(adxl375_test adxl375)

add_executable(avstate_test AvState_test.cpp)
target_link_libraries(avstate_test flight_computer)
target_include_directories(avstate_test PUBLIC ${CMAKE_SOURCE_DIR}/src/flight_control)
# Register the test with CTest
add_test(NAME AvStateTest COMMAND avstate_test)

add_executable(bmi088_test BMI088_test.cpp)
target_link_libraries(bmi088_test bmi08)

add_executable(bmp390_test BMP390_test.cpp)
target_link_libraries(bmp390_test bmp3)

add_executable(gps_test I2CGPS_test.cpp)
target_link_libraries(gps_test gps)

add_executable(lora_test LoRa_test.cpp)
target_link_libraries(lora_test LoRa4Raspi)

add_executable(main_test main_test.cpp)
target_link_libraries(main_test flight_computer)

add_executable(data_test data_test.cpp)
target_link_libraries(data_test flight_computer)
target_include_directories(data_test PUBLIC ${CMAKE_SOURCE_DIR}/src/data)
# Register the test with CTest
add_test(NAME DataTest COMMAND data_test)

# Define ADXL375 Test Executable
add_executable(adxl375_test_mock adxl375_test_mock.cpp)
target_include_directories(adxl375_test_mock
    PRIVATE ${DRIVERS_DIR}
)
target_link_libraries(adxl375_test_mock
    PRIVATE adxl375
    PRIVATE GTest::GTest
    PRIVATE GTest::Main
    PRIVATE GTest::gmock
)

add_library(mock_sensor MockSensor.h)
target_include_directories(mock_sensor PUBLIC  ${DATA_DIR} ${DRIVERS_DIR}
)
target_link_libraries(mock_sensor PRIVATE GTest::GTest GTest::Main GTest::gmock
    PRIVATE adxl375
    PRIVATE bmi08
    PRIVATE bmp3
    PRIVATE gps
    PRIVATE flight_computer
    )


add_executable(mockSensor_test MockSensor_test.cpp)
target_include_directories(mockSensor_test PRIVATE ${DATA_DIR} ${DRIVERS_DIR})
target_link_libraries(mockSensor_test PRIVATE GTest::GTest GTest::Main GTest::gmock
PRIVATE adxl375
PRIVATE bmi08
PRIVATE bmp3
PRIVATE gps
PRIVATE flight_computer)

    
find_package(GTest REQUIRED)
include(GoogleTest)
include_directories(${GTEST_INCLUDE_DIRS})
#target_link_libraries(mock_sensor PUBLIC ${GTEST_LIBRARIES} sensor adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)