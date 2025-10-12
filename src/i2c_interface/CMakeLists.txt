set(I2C_INTERFACE_SOURCES
    i2c_interface.cpp
    i2c_interface.h
    i2c_wrappers.h
)

add_library(i2c_interface ${I2C_INTERFACE_SOURCES})

target_include_directories(i2c_interface PUBLIC ${I2C_INTERFACE_DIR})

target_link_libraries(i2c_interface PUBLIC pigpio)