#Software tests
add_executable(avstate_test AvState_test.cpp)
target_link_libraries(avstate_test flight_computer)
target_include_directories(avstate_test PUBLIC ${CMAKE_SOURCE_DIR}/src/flight_control)
add_test(NAME AvStateTest COMMAND avstate_test)

add_executable(data_logger_test DataLogger_test.cpp)
target_link_libraries(data_logger_test flight_computer)
add_test(NAME DataLoggerTest COMMAND data_logger_test)

add_executable(data_test data_test.cpp)
target_link_libraries(data_test flight_computer)
target_include_directories(data_test PUBLIC ${CMAKE_SOURCE_DIR}/src/data)
add_test(NAME DataTest COMMAND data_test)

add_executable(main_test main_test.cpp)
target_link_libraries(main_test flight_computer)

# Mock Sensor (if needed in software tests)
#set(MOCK_SENSOR src/MockSensor.cpp)
#add_library(mock_sensor ${MOCK_SENSOR})
#target_include_directories(mock_sensor PUBLIC include)
#find_package(GTest REQUIRED)
#include(GoogleTest)
#include_directories(${GTEST_INCLUDE_DIRS})
#target_link_libraries(mock_sensor PUBLIC ${GTEST_LIBRARIES} sensor)
