add_executable(adxl375_test ADXL375_test.cpp)
target_link_libraries(adxl375_test adxl375)

add_executable(avstate_test AvState_test.cpp)
target_link_libraries(avstate_test flight_computer)
target_include_directories(avstate_test PUBLIC ${CMAKE_SOURCE_DIR}/src/flight_control)
# Register the test with CTest
add_test(NAME AvStateTest COMMAND avstate_test)

add_executable(bmi088_test BMI088_test.cpp)
target_link_libraries(bmi088_test bmi08)

add_executable(data_logger_test DataLogger_test.cpp)
target_link_libraries(data_logger_test flight_computer)
add_test(NAME DataLoggerTest COMMAND data_logger_test)

add_executable(bmp390_test BMP390_test.cpp)
target_link_libraries(bmp390_test bmp3)

add_executable(gps_test I2CGPS_test.cpp)
target_link_libraries(gps_test gps)

add_executable(lora_test LoRa_test.cpp)
target_link_libraries(lora_test LoRa4Raspi)
target_include_directories(lora_test PUBLIC ${CMAKE_SOURCE_DIR}/src/)

add_executable(main_test main_test.cpp)
target_link_libraries(main_test flight_computer)

add_executable(ina228_lpb_test INA228_LPB_test.cpp)
target_link_libraries(ina228_lpb_test ina228)

add_executable(ina228_hpb_test INA228_HPB_test.cpp)
target_link_libraries(ina228_hpb_test ina228)

add_executable(TMP1075_test TMP1075_test.cpp)
target_link_libraries(TMP1075_test PRIVATE tmp1075)

add_executable(data_test data_test.cpp)
target_link_libraries(data_test flight_computer)
target_include_directories(data_test PUBLIC ${CMAKE_SOURCE_DIR}/src/data)
# Register the test with CTest
add_test(NAME DataTest COMMAND data_test)

add_executable(trb_test TRB_test.cpp)
target_link_libraries(trb_test flight_computer)

add_executable(buzzer_test buzzer_test.cpp)
target_link_libraries(buzzer_test flight_computer)

add_executable(telecom_test telecom_test.cpp)
target_link_libraries(telecom_test flight_computer)

add_executable(avtimer_test av_timer_test.cpp)
target_link_libraries(avtimer_test flight_computer)


#set(MOCK_SENSOR src/MockSensor.cpp)
#add_library(mock_sensor ${MOCK_SENSOR})
#target_include_directories(mock_sensor PUBLIC include)
#find_package(GTest REQUIRED)
#include(GoogleTest)
#include_directories(${GTEST_INCLUDE_DIRS})
#target_link_libraries(mock_sensor PUBLIC ${GTEST_LIBRARIES} sensor adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)
