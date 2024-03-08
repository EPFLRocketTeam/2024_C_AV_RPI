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

project(flight_computer VERSION 0.1 LANGUAGES C CXX)

# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

include_directories(include)

add_library(ADXL375 src/sensor/adxl375.cpp include/sensor/adxl375.h)
target_include_directories(ADXL375 PUBLIC include/sensor)
target_link_libraries(ADXL375 PRIVATE I2CInterface)

add_library(BMP3 src/sensor/bmp3.cpp include/sensor/bmp3.h)
target_include_directories(BMP3 PUBLIC include/sensor)
target_link_libraries(BMP3 PRIVATE I2CInterface)

add_library(BMI08x src/sensor/bmi08xa.cpp src/sensor/bmi08a.cpp src/sensor/bmi08g.cpp include/sensor/bmi08x.h include/sensor/bmi08.h)
target_include_directories(BMI08x PUBLIC include/sensor)
target_link_libraries(BMI08x PRIVATE I2CInterface)

add_library(I2CInterface src/I2CInterface/I2CInterface.cpp include/I2CInterface/I2CInterface.h include/I2CInterface/i2c_wrappers.h)
target_include_directories(I2CInterface PUBLIC include/I2CInterface)
target_link_libraries(I2CInterface PRIVATE pigpio)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

# Test for he BMP390 sensor
add_executable(BMP390_test test/BMP390_test.cpp)
target_link_libraries(BMP390_test BMP3)

<<<<<<< HEAD
target_link_libraries(main_test pigpio BMP3 BMI08x)
>>>>>>> 182b2ad (Add files via upload)
=======
# Test for the BMI088 sensor
add_executable(BMI088_test test/BMI088_test.cpp)
target_link_libraries(BMI088_test BMI08x)

# Test for the ADXL375 sensor
add_executable(ADXL375_test test/ADXL375_test.cpp)
target_link_libraries(ADXL375_test ADXL375)

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
