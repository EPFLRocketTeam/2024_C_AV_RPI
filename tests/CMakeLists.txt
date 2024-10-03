add_executable(adxl375_test ADXL375_test.cpp)
target_link_libraries(ADXL375_test flight_computer)

add_executable(BMP390_test BMI088_test.cpp)
target_link_libraries(BMP390_test flight_computer)

add_executable(AvState_test AvState_test.cpp)
target_link_libraries(AvState_test flight_computer)

add_executable(LoRa_test LoRa_test.cpp)
target_link_libraries(LoRa_test LoRa4Raspi)

add_executable(BMI088_test BMP390_test.cpp)
target_link_libraries(BMI088_test flight_computer)

add_executable(gps_test I2CGPS_test.cpp)
target_link_libraries(gps_test flight_computer)

add_executable(main_test main_test.cpp)
target_link_libraries(main_test flight_computer)

#set(MOCK_SENSOR src/MockSensor.cpp)
#add_library(mock_sensor ${MOCK_SENSOR})
#target_include_directories(mock_sensor PUBLIC include)
#find_package(GTest REQUIRED)
#include(GoogleTest)
#include_directories(${GTEST_INCLUDE_DIRS})
#target_link_libraries(mock_sensor PUBLIC ${GTEST_LIBRARIES} sensor adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)