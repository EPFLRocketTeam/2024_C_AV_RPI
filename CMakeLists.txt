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

target_include_directories(main PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/h_drivers
    ${PROJECT_SOURCE_DIR}/src/data
    ${PROJECT_SOURCE_DIR}/src/flight_control
)

add_subdirectory(src)
add_subdirectory(LoRa4Raspi)
if(ENABLE_TESTING)
# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)
#print in terminal that testing is enabled
    message(STATUS "Testing is enabled")
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
    add_subdirectory(tests)
endif()

# Enable coverage if the option is set
if(ENABLE_COVERAGE)
    message(STATUS "Coverage is enabled")
    target_compile_options(flight_computer PUBLIC --coverage)
    target_link_libraries(flight_computer PUBLIC --coverage)
endif()

