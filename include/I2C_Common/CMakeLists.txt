add_library(I2C_Common i2c_common.c i2c_common.h)

# Include directories for I2C_Common
target_include_directories(I2C_Common
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(I2C_Common pigpio)