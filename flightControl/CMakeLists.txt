set(AV_STATE_SOURCE src/AvState.cpp)
add_library(av_state ${AV_STATE_SOURCE})
target_include_directories(av_state PUBLIC include)
target_link_libraries(av_state data sensors telecom ERT_RF_Protocol_Interface)
