# New CMake build file for FireHorn

cmake_minimum_required(VERSION 3.22.1)

project(flight_computer VERSION 0.1 LANGUAGES C CXX)

# Options
option(ENABLE_TESTING "Enable testing" OFF)
option(ENABLE_COVERAGE "Enable coverage" OFF)
option(USE_MOCK_SENSORS "Use mock sensor drivers instead of real hardware ones" OFF)

# Output compile commands for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set global compile definitions
add_compile_definitions(RF_PROTOCOL_FIREHORN)

# Interface libraries
add_library(ERT_RF_Protocol_Interface INTERFACE)
target_include_directories(ERT_RF_Protocol_Interface INTERFACE ERT_RF_Protocol_Interface)

add_library(2024_C_AV_INTRANET INTERFACE)
target_include_directories(2024_C_AV_INTRANET INTERFACE 2024_C_AV_INTRANET)

add_library(Capsule INTERFACE)
target_include_directories(Capsule INTERFACE 2024_C_AV-GS_CAPSULE/src)

# Path setup
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/data)
set(FLIGHT_CONTROL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/flight_control)
set(I2C_INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/i2c_interface)
set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_drivers)
set(H_DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/h_drivers)
set(KALMAN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/kalman)

# Main executable
add_executable(main main.cpp)

target_link_libraries(main PUBLIC
    flight_computer
    ERT_RF_Protocol_Interface
    2024_C_AV_INTRANET
    Capsule
    LoRa4Raspi
)

# Only link real sensor libraries if we're not using mocks
if(NOT USE_MOCK_SENSORS)
    target_link_libraries(main PUBLIC
        adxl375
        bmi08
        bmp3
        gps
        ina228
        tmp1075
    )
endif()

target_include_directories(main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    /usr/include/eigen3
    ${DATA_DIR}
    ${H_DRIVERS_DIR}
    ${FLIGHT_CONTROL_DIR}
    ${I2C_INTERFACE_DIR}
    ${DRIVERS_DIR}
    ${KALMAN_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mock 
    ${CMAKE_CURRENT_SOURCE_DIR}/2024_C_AV_INTRANET
)

# Subdirectories
add_subdirectory(src)
add_subdirectory(LoRa4Raspi)

# Testing support
if(ENABLE_TESTING)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Testing is enabled")
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
    add_subdirectory(tests)
endif()

# Coverage support
if(ENABLE_COVERAGE)
    message(STATUS "Coverage is enabled")
    target_compile_options(flight_computer PUBLIC --coverage)
    target_link_libraries(flight_computer PUBLIC --coverage)
endif()

if(USE_MOCK_SENSORS)
    target_compile_definitions(flight_computer PUBLIC MOCK_SENSORS_ENABLED=1)
endif()
