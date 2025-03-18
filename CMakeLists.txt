#New CMake build file for FireHorn
# Enable testing
enable_testing()

cmake_minimum_required(VERSION 3.22.1)

project(flight_computer VERSION 0.1 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(ERT_RF_Protocol_Interface INTERFACE)
target_include_directories(ERT_RF_Protocol_Interface INTERFACE ERT_RF_Protocol_Interface)


add_library(2024_C_AV_INTRANET INTERFACE)
target_include_directories(2024_C_AV_INTRANET INTERFACE 2024_C_AV_INTRANET)


add_library(Capsule INTERFACE)
target_include_directories(Capsule INTERFACE 2024_C_AV-GS_CAPSULE/src)



# Set the RFBG protocol to Firehorn
add_compile_definitions(RF_PROTOCOL_FIREHORN)

add_executable(main main.cpp)

add_subdirectory(src)
add_subdirectory(LoRa4Raspi)

if(ENABLE_TESTING OR ENABLE_TESTING_SOFT OR ENABLE_TESTING_HARD)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Testing is enabled")
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
endif()

if(ENABLE_TESTING)
    message(STATUS "Testing is enabled")
    add_subdirectory(tests)
else()
    if(ENABLE_TESTING_SOFT)
        message(STATUS "Software testing is enabled")
        add_subdirectory(tests/software)
    endif()

    if(ENABLE_TESTING_HARD)
        message(STATUS "Hardware testing is enabled")
        add_subdirectory(tests/hardware)
    endif()
endif()

# Enable coverage if the option is set
if(ENABLE_COVERAGE)
    message(STATUS "Coverage is enabled")
    target_compile_options(flight_computer PUBLIC --coverage)
    target_link_libraries(flight_computer PUBLIC --coverage)
endif()

