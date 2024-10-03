set(DATA_DIR data)
set(FLIGHT_CONTROL_DIR flight_control)
set(I2C_INTERFACE_DIR i2c_interface)
set(DRIVERS_DIR sensor_drivers)

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
    
    ${I2C_INTERFACE_DIR}/i2c_interface.cpp
    ${I2C_INTERFACE_DIR}/i2c_interface.h
    ${I2C_INTERFACE_DIR}/i2c_wrappers.h
    
    ${DRIVERS_DIR}/adxl375.cpp
    ${DRIVERS_DIR}/adxl375.h
    ${DRIVERS_DIR}/bmi08_defs.h
    ${DRIVERS_DIR}/bmi08.h
    ${DRIVERS_DIR}/bmi08a.cpp
    ${DRIVERS_DIR}/bmi08g.cpp
    ${DRIVERS_DIR}/bmi08x.h
    ${DRIVERS_DIR}/bmi08xa.cpp
    ${DRIVERS_DIR}/bmp3_defs.h
    ${DRIVERS_DIR}/bmp3.cpp
    ${DRIVERS_DIR}/bmp3.h
)


add_library(flight_computer ${FIREHORN_FC_SOURCES})
target_include_directories(flight_computer PUBLIC
    ${DATA_DIR}
    ${FLIGHT_CONTROL_DIR}
    ${I2C_INTERFACE_DIR}
    ${DRIVERS_DIR}
)
target_link_libraries(flight_computer PUBLIC ERT_RF_Protocol_Interface Capsule pigpio)