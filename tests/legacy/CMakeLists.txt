add_executable(main_test main_test.cpp)
target_link_libraries(main_test flight_computer)

add_executable(data_test data_test.cpp)
target_link_libraries(data_test flight_computer)
target_include_directories(data_test PUBLIC ${CMAKE_SOURCE_DIR}/src/data)
add_test(NAME DataTest COMMAND data_test)

add_executable(valves_test valves_test.cpp)
target_link_libraries(valves_test flight_computer)

add_executable(GS_debug GS_debug.cpp)
target_link_libraries(GS_debug flight_computer)

