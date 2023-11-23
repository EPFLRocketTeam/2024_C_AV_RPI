#New CMake build file for FireHorn
cmake_minimum_required(VERSION 3.25)

project(sensors_test VERSION 0.1 LANGUAGES C CXX)

include_directories(include)

add_subdirectory(include/BMP3)
add_subdirectory(include/BMI08x)
add_subdirectory(include/I2C_Common)

# Test for he BMP390 sensor
add_executable(BMP390_test test/BMP390_test.cpp)
target_link_libraries(BMP390_test BMP3 I2C_Common)

# Test for the BMI088 sensor
add_executable(BMI088_test test/BMI088_test.cpp)
target_link_libraries(BMI088_test BMI08x I2C_Common)

enable_testing()

# Defining tests
add_test (
    NAME BMP390_test
    COMMAND $<TARGET_FILE:BMP390_test>
)
set_tests_properties(BMP390_test PROPERTIES TIMEOUT 5)
add_test (
    NAME BMI088_test
    COMMAND $<TARGET_FILE:BMI088_test>
)
set_tests_properties(BMI088_test PROPERTIES TIMEOUT 5)