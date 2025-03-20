
option(ENABLE_TESTING "Enable testing" OFF)
option(ENABLE_COVERAGE "Enable coverage" OFF)

set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(FLIGHT_CONTROL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flight_control)
set(H_DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/h_drivers)
set(I2C_INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/i2c_interface)
set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sensor_drivers)
set(KALMAN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/kalman)


add_subdirectory(${I2C_INTERFACE_DIR})
add_subdirectory(${DRIVERS_DIR})


set(FIREHORN_FC_SOURCES
    ${DATA_DIR}/data.cpp
    ${DATA_DIR}/data.h
    ${H_DRIVERS_DIR}/h_driver.h
    ${H_DRIVERS_DIR}/sensors.cpp
    ${H_DRIVERS_DIR}/sensors.h
    ${H_DRIVERS_DIR}/telecom.cpp
    ${H_DRIVERS_DIR}/telecom.h
    ${FLIGHT_CONTROL_DIR}/av_state.cpp
    ${FLIGHT_CONTROL_DIR}/av_state.h
    ${FLIGHT_CONTROL_DIR}/thresholds.h
    ${KALMAN_DIR}/kalman.cpp
    ${KALMAN_DIR}/kalman.h
)   

add_library(flight_computer ${FIREHORN_FC_SOURCES})

target_include_directories(flight_computer PUBLIC
    ${DATA_DIR}
    ${H_DRIVERS_DIR}
    ${FLIGHT_CONTROL_DIR}
    ${I2C_INTERFACE_DIR}
    ${DRIVERS_DIR}
)

target_link_libraries(flight_computer PUBLIC
    ERT_RF_Protocol_Interface
    Capsule
    LoRa4Raspi
    adxl375
    bmi08
    bmp3
    gps
    kalman
)
