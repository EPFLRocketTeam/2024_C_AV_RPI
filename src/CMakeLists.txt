option(ENABLE_TESTING "Enable testing" OFF)
option(ENABLE_COVERAGE "Enable coverage" OFF)
option(USE_MOCK_SENSORS "Use mock sensor drivers instead of real hardware ones" OFF)

set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(FLIGHT_CONTROL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flight_control)
set(I2C_INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/i2c_interface)
set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sensor_drivers)
set(H_DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/h_drivers)
set(KALMAN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/kalman)

add_subdirectory(${I2C_INTERFACE_DIR})
add_subdirectory(${DRIVERS_DIR})

# Core flight computer sources
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
    ${FLIGHT_CONTROL_DIR}/av_state.cpp
    ${FLIGHT_CONTROL_DIR}/av_state.h
    ${FLIGHT_CONTROL_DIR}/thresholds.h
    ${KALMAN_DIR}/kalman.cpp
    ${KALMAN_DIR}/kalman.h
    ${KALMAN_DIR}/rotation_utils.cpp
    ${KALMAN_DIR}/rotation_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/tsdb.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/mock/tsdb.h
)

if(USE_MOCK_SENSORS)
    message(STATUS "Using mock sensor drivers in flight_computer")
    list(APPEND FIREHORN_FC_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockAdxl375.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockBmi08.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockBmp390.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockIna228.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mock/MockTmp1075.cpp
    )
endif()

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
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
    ${CMAKE_CURRENT_SOURCE_DIR}/ERT_RF_Protocol_Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/2024_C_AV_INTRANET
)

target_link_libraries(flight_computer PUBLIC
    ERT_RF_Protocol_Interface
    2024_C_AV_INTRANET
    Capsule
    LoRa4Raspi
)

# Link hardware sensor libs only if not using mocks
if(NOT USE_MOCK_SENSORS)
    target_link_libraries(flight_computer PUBLIC
        2024_C_AV_INTRANET
        Capsule
        LoRa4Raspi
        adxl375
        bmi08
        bmp3
        gps
        ina228
        tmp1075
    )
endif()

# Enable a macro for conditional compilation in C++ code
if(USE_MOCK_SENSORS)
    target_compile_definitions(flight_computer PUBLIC MOCK_SENSORS_ENABLED=1)
endif()
