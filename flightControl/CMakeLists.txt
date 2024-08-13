set(AV_STATE_SOURCE src/AvState.cpp)
add_library(av_state ${AV_STATE_SOURCE})
target_include_directories(av_state PUBLIC include)
target_link_libraries(av_state data sensors telecom ERT_RF_Protocol_Interface)

set(KALMAN_SOURCE src/Kalman.cpp)
add_library(kalman ${KALMAN_SOURCE})
target_include_directories(kalman PUBLIC include include/eigen-3.4.0)
target_link_libraries(kalman av_state eigen eigen_lapack eigen_blas)


