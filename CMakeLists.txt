<<<<<<< HEAD
cmake_minimum_required(VERSION 3.5)

project(FlightControlTest)

# Define the include directories
include_directories(include)

# Define the source files
set(SOURCES 
src/flightControl/AvState.cpp
src/flightControl/AvData.cpp
)

# Create the executable
#add_executable(flight_control_test tests/test_AvState.cpp ${SOURCES})
add_executable(basic_init_test tests/basicTest_avState.cpp ${SOURCES})
=======
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
>>>>>>> 182b2ad (Add files via upload)
