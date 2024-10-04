add_executable(adxl375_test ADXL375_test.cpp)
target_link_libraries(adxl375_test adxl375)

add_executable(avstate_test AvState_test.cpp)
target_link_libraries(avstate_test flight_computer)

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

#set(MOCK_SENSOR src/MockSensor.cpp)
#add_library(mock_sensor ${MOCK_SENSOR})
#target_include_directories(mock_sensor PUBLIC include)
#find_package(GTest REQUIRED)
#include(GoogleTest)
#include_directories(${GTEST_INCLUDE_DIRS})
#target_link_libraries(mock_sensor PUBLIC ${GTEST_LIBRARIES} sensor adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)