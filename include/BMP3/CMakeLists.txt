add_library(BMP3 bmp3.cpp bmp3.h)

target_include_directories(BMP3
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

#include_directories(../I2C_Common)

#target_include_directories(BMP3 PRIVATE ../I2C_Common)
target_link_libraries(BMP3 PRIVATE I2C_Common)