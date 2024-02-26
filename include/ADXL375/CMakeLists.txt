add_library(ADXL375 adxl375.cpp adxl375.h)

target_include_directories(ADXL375
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

#include_directories(../I2C_Common)

#target_include_directories(BMP3 PRIVATE ../I2C_Common)
target_link_libraries(ADXL375 PRIVATE I2C_Common)