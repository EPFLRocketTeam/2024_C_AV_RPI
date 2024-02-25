cmake_minimum_required(VERSION 3.5)

project(FlightControlTest)

# Define the include directories
include_directories(include)

# Define the source files
set(SOURCES 
    src/flightControl/AvState.cpp
    tests/test_AvState.cpp
)

# Create the executable
add_executable(flight_control_test ${SOURCES})
