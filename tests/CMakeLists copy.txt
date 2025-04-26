#Hardware tests
add_executable(adxl375_test ADXL375_test.cpp)
target_link_libraries(adxl375_test adxl375)

add_executable(bmi088_test BMI088_test.cpp)
target_link_libraries(bmi088_test bmi08)

add_executable(bmp390_test BMP390_test.cpp)
target_link_libraries(bmp390_test bmp3)

add_executable(gps_test I2CGPS_test.cpp)
target_link_libraries(gps_test gps)

add_executable(lora_test LoRa_test.cpp)
target_link_libraries(lora_test LoRa4Raspi)
