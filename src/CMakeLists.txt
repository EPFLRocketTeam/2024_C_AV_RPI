
option(ENABLE_TESTING "Enable testing" OFF)
option(ENABLE_COVERAGE "Enable coverage" OFF)

set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(FLIGHT_CONTROL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flight_control)
set(I2C_INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/i2c_interface)
set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sensor_drivers)


add_subdirectory(${I2C_INTERFACE_DIR})
add_subdirectory(${DRIVERS_DIR})


set(FIREHORN_FC_SOURCES
    ${DATA_DIR}/data.cpp
    ${DATA_DIR}/data.h
    ${DATA_DIR}/sensors.cpp
    ${DATA_DIR}/sensors.h
    ${DATA_DIR}/telecom.cpp
    ${DATA_DIR}/telecom.h
    
    ${FLIGHT_CONTROL_DIR}/av_state.cpp
    ${FLIGHT_CONTROL_DIR}/av_state.h
    ${FLIGHT_CONTROL_DIR}/thresholds.h
)

add_library(flight_computer ${FIREHORN_FC_SOURCES})

target_include_directories(flight_computer PUBLIC
    ${DATA_DIR}
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
)

# Enable testing if the option is set
if(ENABLE_TESTING)
#print in terminal that testing is enabled
    message(STATUS "Testing is enabled")
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
endif()

# Enable coverage if the option is set
if(ENABLE_COVERAGE)
    message(STATUS "Coverage is enabled")
    target_compile_options(flight_computer PUBLIC --coverage)
    target_link_libraries(flight_computer PUBLIC --coverage)
endif()