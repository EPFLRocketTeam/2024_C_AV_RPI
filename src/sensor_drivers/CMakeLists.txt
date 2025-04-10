if(NOT USE_MOCK_SENSORS)

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

    # INA228
    set(INA228_SOURCES
        INA228.cpp
        INA228.h
    )
    add_library(ina228 ${INA228_SOURCES})
    target_include_directories(ina228 INTERFACE ${DRIVERS_DIR})
    target_link_libraries(ina228 PRIVATE i2c_interface)

    # TMP1075
    set(TMP1075_SOURCES
        TMP1075.cpp
        TMP1075.h
    )
    add_library(tmp1075 ${TMP1075_SOURCES})
    target_include_directories(tmp1075 INTERFACE ${DRIVERS_DIR})
    target_link_libraries(tmp1075 PRIVATE i2c_interface)

endif()

if(USE_MOCK_SENSORS)
    # Mock ADXL375
    add_library(MockAdxl375 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockAdxl375.cpp)
    target_include_directories(MockAdxl375 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mock)

    # Mock BMI08
    add_library(MockBmi08 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockBmi08.cpp)
    target_include_directories(MockBmi08 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mock)

    # Mock BMP3
    add_library(MockBmp390 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockBmp390.cpp)
    target_include_directories(MockBmp390 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mock)

    # Mock GPS
    add_library(MockGps STATIC ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockGps.cpp)
    target_include_directories(MockGps INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mock)

    # Mock INA228
    add_library(MockIna228 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockIna228.cpp)
    target_include_directories(MockIna228 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mock)

    # Mock TMP1075
    add_library(MockTmp1075 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockTmp1075.cpp)
    target_include_directories(MockTmp1075 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/mock)
endif()

