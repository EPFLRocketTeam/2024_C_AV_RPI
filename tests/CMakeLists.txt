set (ADXL375_TEST src/ADXL375_test.cpp)
add_executable(ADXL375_test ${ADXL375_TEST})
target_link_libraries(ADXL375_test adxl375)

set (BMP390_TEST src/BMP390_test.cpp)
add_executable(BMP390_test ${BMP390_TEST})
target_link_libraries(BMP390_test bmp3)

set (AV_STATE_TEST src/AvState_test.cpp)
add_executable(AvState_test ${AV_STATE_TEST})
target_link_libraries(AvState_test av_state)

set (LORA_TEST src/LoRa_test.cpp)
add_executable(LoRa_test ${LORA_TEST})
target_link_libraries(LoRa_test )
# Find the pigpio library
find_library(PIGPIO_LIBRARY pigpio)
# Link the pigpio library, the real-time library (rt), and the pthread library
target_link_libraries(LoRa_test ${PIGPIO_LIBRARY} LoRa rt pthread Wstring itoa LoopbackStream Print Stream)


set (BMI088_TEST src/BMI088_test.cpp)
add_executable(BMI088_test ${BMI088_TEST})
target_link_libraries(BMI088_test PUBLIC bmi08xa)

set (I2CGPS_SOURCE src/I2CGPS_test.cpp)
add_executable(gps_test ${I2CGPS_SOURCE})
target_link_libraries(gps_test PUBLIC I2CGPS TinyGPS++)

set (MAIN_TEST src/main_test.cpp)
add_executable(main_test ${MAIN_TEST})
target_link_libraries(main_test PUBLIC
        data sensors telecom ERT_RF_Protocol_Interface
        adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)

#set(MOCK_SENSOR src/MockSensor.cpp)
#add_library(mock_sensor ${MOCK_SENSOR})
#target_include_directories(mock_sensor PUBLIC include)
#find_package(GTest REQUIRED)
#include(GoogleTest)
#include_directories(${GTEST_INCLUDE_DIRS})
#target_link_libraries(mock_sensor PUBLIC ${GTEST_LIBRARIES} sensor adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)