# Define individual libraries for each sensor
# ADXL375
add_library(adxl375 adxl375.cpp adxl375.h)
target_include_directories(adxl375 INTERFACE ${DRIVERS_DIR})
target_link_libraries(adxl375 PRIVATE i2c_interface)

# BMI08
set(BMI08_SOURCES
    bmi08_defs.h
    bmi08.h
    bmi08a.cpp
    bmi08g.cpp
    bmi08x.h
    bmi08xa.cpp
)
add_library(bmi08 ${BMI08_SOURCES})
target_include_directories(bmi08 INTERFACE ${DRIVERS_DIR})
target_link_libraries(bmi08 PRIVATE i2c_interface)

# BMP3
set(BMP3_SOURCES
    bmp3_defs.h
    bmp3.cpp
    bmp3.h
)
add_library(bmp3 ${BMP3_SOURCES})
target_include_directories(bmp3 INTERFACE ${DRIVERS_DIR})
target_link_libraries(bmp3 PRIVATE i2c_interface)

# GPS
set(GPS_SOURCES
    I2CGPS.cpp
    I2CGPS.h
    TinyGPS++.cpp
    TinyGPS++.h
)
add_library(gps ${GPS_SOURCES})
target_include_directories(gps INTERFACE ${DRIVERS_DIR})
target_link_libraries(gps PRIVATE i2c_interface)
# TMP1075
set(TMP1075_SOURCES
    TMP1075.cpp
    TMP1075.h
)
add_library(tmp1075 ${TMP1075_SOURCES})
target_include_directories(tmp1075 INTERFACE ${DRIVERS_DIR})
target_link_libraries(tmp1075 PRIVATE i2c_interface)