// /**\
//  * Author: Alexis Limozin
//  *
//  * 
//  **/

// #include <iostream>
// #include <cassert>
// #include <unistd.h>
// //  #include "bmi08x.h"
// #include <fstream>

// //  #include "adxl375.h"
// #include "MockAdxl375.h"
// #include "MockBmi08.h"

// #include "kalman.h"
// #include "kalman_params.h"
// #include <chrono>
// #include "data.h"
// #include "tsdb.h"
// /************************************************************************/
// /*********                     Macros                              ******/
// /************************************************************************/
// /* Iteration count to run example code */
// #define ITERATION  UINT16_C(60000)
// #define RAD_TO_DEG 57.2957795131f
// namespace {
//     auto start = std::chrono::high_resolution_clock::now();

//     uint32_t millis() {
//         // Calculate the elapsed time, converting duration to milliseconds
//         auto now = std::chrono::high_resolution_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now-start);
//         return static_cast<uint32_t>(duration.count());
//     };

//     uint32_t micros() {
//         // Calculate the elapsed time, converting duration to microseconds
//         auto now = std::chrono::high_resolution_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
//         return static_cast<uint32_t>(duration.count());
//     };

// }

// /************************************************************************/
// /*********                     Test code                           ******/
// /************************************************************************/
 
 
//  int main(void) {
//     //std::ofstream log("/boot/av_std::cout/fusion_test.csv");
//     //if (!log.is_open()) {
//     //    std::cerr << "Failed to open std::cout file" << std::endl;
//     //    return 1;
//     //}
 
//     //  std::cout << "time (us), ADXL_Prim_X, ADXL_Prim_Y, ADXL_Prim_Z, ADXL_Sec_X, ADXL_Sec_Y, ADXL_Sec_Z, "
//     //      << "BMI_Prim_Acc_X, BMI_Prim_Acc_Y, BMI_Prim_Acc_Z, BMI_Prim_Gyr_X, BMI_Prim_Gyr_Y, BMI_Prim_Gyr_Z, "
//     //      << "BMI_Sec_Acc_X, BMI_Sec_Acc_Y, BMI_Sec_Acc_Z, BMI_Sec_Gyr_X, BMI_Sec_Gyr_Y, BMI_Sec_Gyr_Z, "
//     //      << "Azimuth, Pitch" << std::endl;

//     // test only with one imu
//     // std::cout << "time (us), "
//     //      << "BMI_Prim_Acc_X, BMI_Prim_Acc_Y, BMI_Prim_Acc_Z, BMI_Prim_Gyr_X, BMI_Prim_Gyr_Y, BMI_Prim_Gyr_Z, "
//     //      << "Azimuth, Pitch" << std::endl;
 
//     int loop = 0;
//     try {
//         bmi08_sensor_data_f dataBM;

//         std::optional<TimeSeries> bmi1_acc_x, bmi1_acc_y, bmi1_acc_z;
//         std::optional<TimeSeries> bmi1_gyro_x, bmi1_gyro_y, bmi1_gyro_z;


//         // print the current working directory
//         char cwd[1024];
//         if (getcwd(cwd, sizeof(cwd)) != nullptr) {
//             std::cout << "Current working dir: " << cwd << std::endl;
//         } else {
//             perror("getcwd() error");
//             return 1;
//         }

//         TDB *tdb = nullptr;
//         tdb = TDB::from_csv("../tests/NavigationSensors_.csv");

 
//         Kalman kalman(INITIAL_COV_GYR_BIAS,
//                     INITIAL_COV_ACCEL_BIAS,
//                     INITIAL_COV_ORIENTATION,
//                     GYRO_COV,
//                     GYRO_BIAS_COV,
//                     ACCEL_COV,
//                     ACCEL_BIAS_COV,
//                     GPS_OBS_COV,
//                     ALT_OBS_COV);
//         kalman.set_is_static(true);
//         std::cout << "kalman initialized" << std::endl;
 
//         uint32_t last_print_time = millis();
//         bool first_non_static_flag = false;

//         uint32_t max_predict_time = 0;

//         NavSensors nav_sensors;
//         NavigationData nav_data;

//         nav_data.position.lat = 0;
//         nav_data.position.lng = 0;

//         usleep(1000);
//         tdb->init();

//         std::cout << "tdb initialized" << std::endl;

//         bmi1_acc_x = tdb->get_time_series("bmi1_acc_x");
//         bmi1_acc_y = tdb->get_time_series("bmi1_acc_y");
//         bmi1_acc_z = tdb->get_time_series("bmi1_acc_z");

//         bmi1_gyro_x = tdb->get_time_series("bmi1_gyro_x");
//         bmi1_gyro_y = tdb->get_time_series("bmi1_gyro_y");
//         bmi1_gyro_z = tdb->get_time_series("bmi1_gyro_z");

//         std::cout << "time series initialized" << std::endl;

//         while (loop < ITERATION) {
//             // if (adxl1.get_status() & ADXL375_DATA_READY) {
//             //     dataAD = adxl1.get_data();
//             //     nav_sensors.adxl = dataAD;
//             // }

//             // if (adxl2.get_status() & ADXL375_DATA_READY) {
//             //     dataAD = adxl2.get_data();
//             //     nav_sensors.adxl_aux = dataAD;
//             // }

//             // if (bmi1.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
//             //     dataBM = bmi1.get_accel_data();
//             //     nav_sensors.bmi_accel = dataBM;
//             //     dataBM = bmi1.get_gyro_data();
//             //     nav_sensors.bmi_gyro = dataBM;
//             // }

//             // if (bmi2.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
//             //     dataBM = bmi2.get_accel_data();
//             //     nav_sensors.bmi_aux_accel = dataBM;
//             //     dataBM = bmi2.get_gyro_data();
//             //     nav_sensors.bmi_aux_gyro = dataBM;
//             // }

//             dataBM = {
//                 bmi1_acc_x.value().get(),
//                 bmi1_acc_y.value().get(),
//                 bmi1_acc_z.value().get()};
//             nav_sensors.bmi_accel = dataBM;

//             dataBM = {
//                 bmi1_gyro_x.value().get(),
//                 bmi1_gyro_y.value().get(),
//                 bmi1_gyro_z.value().get()};
//             nav_sensors.bmi_gyro = dataBM;

            
//             // Start timing
//             auto start_time = std::chrono::high_resolution_clock::now();

//             // Call the function
//             kalman.predict(nav_sensors, nav_data);

//             // Stop timing
//             auto end_time = std::chrono::high_resolution_clock::now();

//             // Calculate the duration
//             auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
//             if (duration.count() > max_predict_time) {
//                 max_predict_time = duration.count();
//             }

//             nav_data = kalman.get_nav_data();

//             loop++;
//             usleep(1000);
//             if (millis() - last_print_time > 500 || loop < 10) { // print at 2Hz
//                 std::cout << micros() << ","
//                 << nav_data.position_kalman.x << "," << nav_data.position_kalman.y << "," << nav_data.position_kalman.z << ","
//                 << kalman.get_azimuth() * RAD_TO_DEG << "," << kalman.get_pitch() * RAD_TO_DEG<< std::endl;
//                 // std::cout << "kalman.predict() at max took " << max_predict_time << " microseconds." << std::endl;

//                 last_print_time = millis();
//             } 

//             if(nav_sensors.bmi_accel.z > 14.0f && not first_non_static_flag){
//             kalman.set_is_static(false);
//             std::cout << "Rocket is not static anymore at loop " << loop 
//                         << " and timestamp " << micros() << " us" << std::endl;
//             first_non_static_flag = true;
//             }
//         }
//     } catch (const std::exception& e) {
//         std::cerr << e.what() << std::endl;
//         //log.close();
//         return 1;
//     }
//     //log.close();
//     return 0;
// }