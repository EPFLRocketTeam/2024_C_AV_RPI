#New CMake build file for FireHorn
cmake_minimum_required(VERSION 3.22.1)

project(flight_computer VERSION 0.1 LANGUAGES C CXX)
add_library(ERT_RF_Protocol_Interface INTERFACE)
target_include_directories(ERT_RF_Protocol_Interface INTERFACE ERT_RF_Protocol_Interface)
add_library(Capsule INTERFACE)
target_include_directories(Capsule INTERFACE 2024_C_AV-GS_CAPSULE/src)
# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

# Set the RFBG protocol to Firehorn
add_compile_definitions(RF_PROTOCOL_FIREHORN)

add_executable(main main.cpp)

add_subdirectory(data)
add_subdirectory(flightControl)
add_subdirectory(I2CInterface)
add_subdirectory(LoRa4Raspi)
add_subdirectory(sensor)
add_subdirectory(tests)

