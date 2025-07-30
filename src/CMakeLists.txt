option(ENABLE_TESTING "Enable testing" OFF)
option(ENABLE_COVERAGE "Enable coverage" OFF)

set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(FLIGHT_CONTROL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flight_control)
set(I2C_INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/i2c_interface)
set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sensor_drivers)
set(H_DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/h_drivers)
set(KALMAN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/kalman)
set(MODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(MODULES_SENSORS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/sensors)

add_subdirectory(${I2C_INTERFACE_DIR})
add_subdirectory(${DRIVERS_DIR})
# add_subdirectory(${KALMAN_DIR})

set(FIREHORN_FC_SOURCES
    ${DATA_DIR}/data.cpp
    ${DATA_DIR}/data.h
    ${DATA_DIR}/logger.cpp
    ${DATA_DIR}/logger.h
    ${H_DRIVERS_DIR}/h_driver.h
    ${H_DRIVERS_DIR}/PR_board.cpp
    ${H_DRIVERS_DIR}/PR_board.h
    ${H_DRIVERS_DIR}/trigger_board.cpp
    ${H_DRIVERS_DIR}/trigger_board.h
    ${H_DRIVERS_DIR}/sensors.cpp
    ${H_DRIVERS_DIR}/sensors.h
    ${H_DRIVERS_DIR}/telecom.cpp
    ${H_DRIVERS_DIR}/telecom.h
    ${H_DRIVERS_DIR}/dpr.cpp
    ${H_DRIVERS_DIR}/dpr.h
    ${H_DRIVERS_DIR}/camera.cpp
    ${H_DRIVERS_DIR}/camera.h
    ${FLIGHT_CONTROL_DIR}/av_state.cpp
    ${FLIGHT_CONTROL_DIR}/av_state.h
    ${FLIGHT_CONTROL_DIR}/thresholds.h
    ${KALMAN_DIR}/kalman.cpp
    ${KALMAN_DIR}/kalman.h
    ${KALMAN_DIR}/rotation_utils.h
    ${KALMAN_DIR}/rotation_utils.cpp
    ${MODULES_DIR}/dynconf.cpp
    ${MODULES_DIR}/module.cpp
    ${MODULES_DIR}/dynconf.h
    ${MODULES_DIR}/module.h
    ${MODULES_SENSORS_DIR}/adxl375_module.cpp
    ${MODULES_SENSORS_DIR}/bmi088_module.cpp
    ${MODULES_SENSORS_DIR}/bmp390_module.cpp
    ${MODULES_SENSORS_DIR}/gps_module.cpp
    ${MODULES_SENSORS_DIR}/ina228_module.cpp
    ${MODULES_SENSORS_DIR}/tmp1075_module.cpp
    ${MODULES_SENSORS_DIR}/adxl375_module.h
    ${MODULES_SENSORS_DIR}/bmi088_module.h
    ${MODULES_SENSORS_DIR}/bmp390_module.h
    ${MODULES_SENSORS_DIR}/gps_module.h
    ${MODULES_SENSORS_DIR}/ina228_module.h
    ${MODULES_SENSORS_DIR}/tmp1075_module.h
    ${CMAKE_CURRENT_SOURCE_DIR}/buzzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buzzer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/av_timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/av_timer.h
)

add_library(flight_computer ${FIREHORN_FC_SOURCES})

target_include_directories(flight_computer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    /usr/include/eigen3
    ${DATA_DIR}
    ${H_DRIVERS_DIR}
    ${FLIGHT_CONTROL_DIR}
    ${I2C_INTERFACE_DIR}
    ${DRIVERS_DIR}
    ${KALMAN_DIR}
    ${MODULES_DIR}
    ${MODULES_SENSORS_DIR}
)

target_link_libraries(flight_computer PUBLIC
    ERT_RF_Protocol_Interface
    2024_C_AV_INTRANET
    Capsule
    LoRa4Raspi
    adxl375
    bmi08
    bmp3
    gps
    ina228
    tmp1075
      # Kalman should now be linked properly
)
