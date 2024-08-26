
set( DATA_SOURCES src/data.cpp)
add_library(data ${DATA_SOURCES})
target_include_directories(data PUBLIC include)
target_link_libraries(data ERT_RF_Protocol_Interface LoopbackStream LoRa  adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)

set(SENSORS_SOURCES src/sensors.cpp)
add_library(sensors ${SENSORS_SOURCES})
target_include_directories(sensors PUBLIC include)
include_directories(../sensor/include)
target_link_libraries(sensors adxl375 bmi08a bmi08g bmi08xa bmp3 I2CGPS TinyGPS++)

set(TELECOM_SOURCES src/telecom.cpp)
add_library(telecom ${TELECOM_SOURCES})
target_include_directories(telecom PUBLIC include)
target_link_libraries(telecom ERT_RF_Protocol_Interface LoopbackStream LoRa)