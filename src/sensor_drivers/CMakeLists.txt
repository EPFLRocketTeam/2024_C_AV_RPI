# Define ADXL375 Library
add_library(adxl375 adxl375.cpp adxl375.h adxl375_gmock.h adxl375_interface.h)
target_include_directories(adxl375
    INTERFACE ${DRIVERS_DIR}
)
target_link_libraries(adxl375 PRIVATE i2c_interface)

# Define BMI08 Library
# Define BMI08 Library
set(BMI08_SOURCES
    bmi08_defs.h
    bmi08.h
    bmi08a.cpp
    bmi08g.cpp
    bmi08x.h
    bmi08xa.cpp
    bmi08_gmock.h
    bmi08_interface.h
)
add_library(bmi08 ${BMI08_SOURCES})
target_include_directories(bmi08
    INTERFACE ${DRIVERS_DIR}
)
target_include_directories(bmi08
    INTERFACE ${DRIVERS_DIR}
)
target_link_libraries(bmi08 PRIVATE i2c_interface)

# Define BMP3 Library
# Define BMP3 Library
set(BMP3_SOURCES
    bmp3_defs.h
    bmp3.cpp
    bmp3.h
    bmp3_interface.h
    bmp3_gmock.h
)
add_library(bmp3 ${BMP3_SOURCES})
target_include_directories(bmp3
    INTERFACE ${DRIVERS_DIR}
)
target_include_directories(bmp3
    INTERFACE ${DRIVERS_DIR}
)
target_link_libraries(bmp3 PRIVATE i2c_interface)

# Define GPS Library
# Define GPS Library
set(GPS_SOURCES
    I2CGPS.cpp
    I2CGPS.h
    TinyGPS++.cpp
    TinyGPS++.h
    TinyGPS++_interface.h
    TinyGPS++_gmock.h
    I2CGPS_gmock.h
    I2CGPS_interface.h
)
add_library(gps ${GPS_SOURCES})
target_include_directories(gps
    INTERFACE ${DRIVERS_DIR}
)
target_include_directories(gps
    INTERFACE ${DRIVERS_DIR}
)
target_link_libraries(gps PRIVATE i2c_interface)

