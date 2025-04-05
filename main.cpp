#include <iostream>
// #include <memory>
// #include <chrono>
// #include <thread>
// #include "sensors.h"
// #include "data.h"
// #include "av_state.h"

// int main() {
//     try {
//         AvState state;
//         DataDump dump = Data::get_instance().get();
//         dump.av_state = State::READY;
//         Data::get_instance().write(Data::AV_STATE, &dump.av_state);

//         std::unique_ptr<Sensors> sensors = std::make_unique<Sensors>();

//         std::cout << "[INFO] Sensors initialized. Beginning simulation loop\n";

//         for (int i = 0; i < 10; ++i) {
//             auto now = std::chrono::steady_clock::now();
//             auto timestamp_gen = static_cast<uint32_t>(
//                 std::chrono::duration_cast<std::chrono::milliseconds>(
//                     now.time_since_epoch()).count());
//             Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp_gen);

//             sensors->check_policy(dump, 100); 

//             adxl375_data adxl_data;
//             if (Data::get_instance().read(Data::NAV_SENSOR_ADXL1_DATA, &adxl_data)) {
//                 std::cout << "[ADXL1] X: " << adxl_data.x
//                           << ", Y: " << adxl_data.y
//                           << ", Z: " << adxl_data.z << std::endl;
//             }

//             std::this_thread::sleep_for(std::chrono::milliseconds(100));
//         }

//     } catch (const std::exception& e) {
//         std::cerr << "[ERROR] Exception in sensors test: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }

//     return EXIT_SUCCESS;
// }
