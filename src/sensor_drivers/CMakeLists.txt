# Set C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate Google Test
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)


# Define ADXL375 Library
add_library(adxl375 adxl375.cpp adxl375.h)
target_include_directories(adxl375
    INTERFACE ${DRIVERS_DIR}
    PRIVATE "../i2c_interface"
)
#target_link_libraries(adxl375 PRIVATE i2c_interface)

# Define BMI08 Library
set(BMI08_SOURCES
    bmi08_defs.h
    bmi08.h
    bmi08a.cpp
    bmi08g.cpp
    bmi08x.h
    bmi08xa.cpp
)
add_library(bmi08 ${BMI08_SOURCES})
target_include_directories(bmi08
    INTERFACE ${DRIVERS_DIR}
    PRIVATE "../i2c_interface"
)
#target_link_libraries(bmi08 PRIVATE i2c_interface)

# Define BMP3 Library
set(BMP3_SOURCES
    bmp3_defs.h
    bmp3.cpp
    bmp3.h
)
add_library(bmp3 ${BMP3_SOURCES})
target_include_directories(bmp3
    INTERFACE ${DRIVERS_DIR}
    PRIVATE "../i2c_interface"
)
#target_link_libraries(bmp3 PRIVATE i2c_interface)

# Define GPS Library
set(GPS_SOURCES
    I2CGPS.cpp
    I2CGPS.h
    TinyGPS++.cpp
    TinyGPS++.h
)
add_library(gps ${GPS_SOURCES})
target_include_directories(gps
    INTERFACE ${DRIVERS_DIR}
    PRIVATE "../i2c_interface"
)
#target_link_libraries(gps PRIVATE i2c_interface)

# Define ADXL375 Test Executable
add_executable(adxl375_test adxl375_test.cpp adxl375_gmock.h)
target_include_directories(adxl375_test
    PRIVATE ${DRIVERS_DIR}
    PRIVATE "../i2c_interface"
    PRIVATE ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(adxl375_test
    PRIVATE adxl375
    PRIVATE GTest::GTest
    PRIVATE GTest::Main
)