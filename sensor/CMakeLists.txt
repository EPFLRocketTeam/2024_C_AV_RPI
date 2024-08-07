set(ADXL375_SOURCE src/adxl375.cpp)
add_library(adxl375 ${ADXL375_SOURCE})
include_directories(../I2CInterface/include)
target_link_libraries(adxl375 I2CInterface)
target_include_directories(adxl375 PUBLIC include)

set(BMI08A_SOURCE src/bmi08a.cpp)
add_library(bmi08a ${BMI08A_SOURCE})
target_link_libraries(bmi08a I2CInterface)
target_include_directories(bmi08a PUBLIC include)

set(BMI08G_SOURCE src/bmi08g.cpp)
add_library(bmi08g ${BMI08G_SOURCE})
target_link_libraries(bmi08g I2CInterface)
target_include_directories(bmi08g PUBLIC include)

set(BMI08XA_SOURCE src/bmi08xa.cpp)
add_library(bmi08xa ${BMI08XA_SOURCE})
target_include_directories(bmi08xa PUBLIC include)
target_link_libraries(bmi08xa I2CInterface)
include_directories(../I2CInterface/include)

set(BMP3_SOURCE src/bmp3.cpp)
add_library(bmp3 ${BMP3_SOURCE})
target_link_libraries(bmp3 I2CInterface)
target_include_directories(bmp3 PUBLIC include)

set(I2CPGS_SOURCE src/I2CGPS.cpp)
add_library(I2CGPS ${I2CPGS_SOURCE})
target_link_libraries(I2CGPS I2CInterface)
target_include_directories(I2CGPS PUBLIC include)

set(TinyGPS_SOURCE src/TinyGPS++.cpp)
add_library(TinyGPS++ ${TinyGPS_SOURCE})
target_link_libraries(TinyGPS++ I2CInterface)
target_include_directories(TinyGPS++ PUBLIC include)


