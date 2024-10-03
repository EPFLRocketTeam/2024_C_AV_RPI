add_library(adxl375 src/adxl375.cpp)
target_include_directories(adxl375 PUBLIC include)
target_link_libraries(adxl375 I2CInterface)

set(BMI08_SOURCES src/bmi08a.cpp
                  src/bmi08g.cpp
                  src/bmi08xa.cpp)
add_library(bmi08 ${BMI08_SOURCES})
target_include_directories(bmi08 PUBLIC include)
target_link_libraries(bmi08 PUBLIC I2CInterface)

add_library(bmp3 src/bmp3.cpp)
target_include_directories(bmp3 PUBLIC include)
target_link_libraries(bmp3 I2CInterface)

set(I2CPGS_SOURCE src/I2CGPS.cpp
                  src/TinyGPS++.cpp)
add_library(i2cgps ${I2CPGS_SOURCE})
target_include_directories(i2cgps PUBLIC include)
target_link_libraries(i2cgps I2CInterface)
