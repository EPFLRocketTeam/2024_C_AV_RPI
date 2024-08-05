set (ADXL375_TEST src/ADXL375_test.cpp)
add_executable(ADXL375_test ${ADXL375_TEST})
target_link_libraries(ADXL375_test adxl375)

set (BMI088_TEST src/BMI088_test.cpp)
add_executable(BMI088_test ${BMI088_TEST})
target_link_libraries(BMI088_test)
include_directories(../sensor/include)

set (BMP390_TEST src/BMP390_test.cpp)
add_executable(BMP390_test ${BMP390_TEST})
target_link_libraries(BMP390_test bmp3)

set (LORA_TEST src/LoRa_test.cpp)
add_executable(LoRa_test ${LORA_TEST})
target_link_libraries(LoRa_test LoRa)

set (AV_STATE_TEST src/AvState_test.cpp)
add_executable(AvState_test ${AV_STATE_TEST})
target_link_libraries(AvState_test av_state)
