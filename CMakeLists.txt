#New CMake build file for FireHorn
cmake_minimum_required(VERSION 3.22.1)



project(flight_computer VERSION 0.1 LANGUAGES C CXX)
add_library(ERT_RF_Protocol_Interface INTERFACE)
target_include_directories(ERT_RF_Protocol_Interface INTERFACE ERT_RF_Protocol_Interface)
# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

add_executable(main main.cpp)

add_subdirectory(data)
add_subdirectory(flightControl)
add_subdirectory(I2CInterface)
add_subdirectory(LoRa4Raspi)
add_subdirectory(sensor)
add_subdirectory(tests)

