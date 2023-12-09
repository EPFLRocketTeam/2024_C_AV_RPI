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

project(sensors_test VERSION 0.1 LANGUAGES C CXX)

# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

include_directories(include)

add_subdirectory(include/BMP3)
add_subdirectory(include/BMI08x)
add_subdirectory(include/ADXL375)
add_subdirectory(include/I2C_Common)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

# Test for he BMP390 sensor
add_executable(BMP390_test test/BMP390_test.cpp)
target_link_libraries(BMP390_test BMP3 I2C_Common)

<<<<<<< HEAD
target_link_libraries(main_test pigpio BMP3 BMI08x)
>>>>>>> 182b2ad (Add files via upload)
=======
# Test for the BMI088 sensor
add_executable(BMI088_test test/BMI088_test.cpp)
target_link_libraries(BMI088_test BMI08x I2C_Common)

# Test for the ADXL375 sensor
add_executable(ADXL375_test test/ADXL375_test.cpp)
target_link_libraries(ADXL375_test ADXL375 I2C_Common)

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
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 4db086c (Changed CMakeLists)
=======
set_tests_properties(BMI088_test PROPERTIES TIMEOUT 5)
>>>>>>> 3cbf43e (Interrupts for accelerometer working now)
=======
set_tests_properties(BMI088_test PROPERTIES TIMEOUT 5)

add_test (
    NAME ADXL375_test
    COMMAND $<TARGET_FILE:ADXL375_test>
)
set_tests_properties(ADXL375_test PROPERTIES TIMEOUT 5)
>>>>>>> c7df872 (Made the test for adxl375)
