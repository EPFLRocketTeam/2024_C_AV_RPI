set(I2CINTERFACE_SOURCE src/I2CInterface.cpp)
add_library(I2CInterface ${I2CINTERFACE_SOURCE})
target_include_directories(I2CInterface PUBLIC include)

