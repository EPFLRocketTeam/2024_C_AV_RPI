#New CMake build file for FireHorn
cmake_minimum_required(VERSION 3.25)

project(main_test VERSION 0.1 LANGUAGES C CXX)

include_directories(include)

add_subdirectory(include/BMP3)
add_subdirectory(include/BMI08x)
add_subdirectory(include/I2C_Common)

set(SOURCES main.cpp)
add_executable(main_test ${SOURCES})

target_link_libraries(main_test pigpio BMP3 BMI08x)