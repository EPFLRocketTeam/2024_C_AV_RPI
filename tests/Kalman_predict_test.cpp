#include <iostream>
#include <iomanip> // For std::fixed and std::setprecision
#include <unistd.h> // For usleep
#include <chrono>   // For millis/micros
#include <limits>   // For std::numeric_limits for NaN checks if needed for printing

// Project headers
#include "bmi08x.h"
#include "adxl375.h"
#include "kalman.h"
#include "data.h"
#include "kalman_params.h" // For DEG_TO_RAD, INITIAL_AZIMUTH etc.

// Helper functions for timing, similar to kalman.cpp
namespace {
    auto start_time_chrono = std::chrono::high_resolution_clock::now();

    uint32_t millis() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_chrono);
        return static_cast<uint32_t>(duration.count());
    }

    uint32_t micros() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time_chrono);
        return static_cast<uint32_t>(duration.count());
    }
}

int main(void) {
    // Sensor Initialization
    Bmi088 bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
    Bmi088 bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY);
    Adxl375 adxl1(ADXL375_ADDR_I2C_PRIM);
    Adxl375 adxl2(ADXL375_ADDR_I2C_SEC);

    // Kalman Initialization with example parameters
    Kalman kalman(INITIAL_COV_GYR_BIAS,
           INITIAL_COV_ACCEL_BIAS,
           INITIAL_COV_ORIENTATION,
           GYRO_COV,
           GYRO_BIAS_COV,
           ACCEL_COV,
           ACCEL_BIAS_COV,
           GPS_OBS_COV,
           ALT_OBS_COV);

    NavSensors nav_sensors;
    NavigationData nav_data; // Will be mostly empty/default for this predict-only test

    // Set dummy GPS and pressure data for Kalman's initial calibration if it uses them
    nav_data.position.lat = 0.0;
    nav_data.position.lng = 0.0;
    // Standard sea-level pressure in Pa, then converted to hPa for BMP data struct
    nav_sensors.bmp.pressure = 1013.25f; 
    nav_sensors.bmp_aux.pressure = 1013.25f;


    const uint32_t KALMAN_CALIB_DURATION_MS = 5000; // 5 seconds for Kalman static calibration
    const uint32_t TOTAL_DURATION_MS = 20 * 1000; // Run for 20 seconds (20000 ms)

    std::cout << "Starting Kalman predict test for " << TOTAL_DURATION_MS / 1000 << " seconds..." << std::endl;
    std::cout << "Kalman static calibration will run for the first " << KALMAN_CALIB_DURATION_MS / 1000 << " seconds." << std::endl;

    // Print CSV header
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Time_ms,Azimuth_deg,Pitch_deg,BMI1_ax,BMI1_ay,BMI1_az,BMI2_ax,BMI2_ay,BMI2_az,Fused_ax,Fused_ay,Fused_az,cBMI_x,cBMI_y,cBMI_z,cBMI_aux_x,cBMI_aux_y,cBMI_aux_z" << std::endl;

    // Initialize Kalman's last_measurement_time by calling micros once before the loop
    // This is handled internally by Kalman's first predict call where last_measurement_time is 0.
    bool switched_to_non_static_printed = false;
    start_time_chrono = std::chrono::high_resolution_clock::now(); // Reset start time just before the loop

    while (millis() < TOTAL_DURATION_MS) {
        uint32_t current_ms = millis();

        // Read sensor data
        bmi08_sensor_data_f acc1_data, gyro1_data, acc2_data, gyro2_data;
        adxl375_data adxl1_data, adxl2_data;

        if (bmi1.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
            acc1_data = bmi1.get_accel_data();
        }
        if (bmi1.get_gyro_status() & BMI08_GYRO_DATA_READY_INT) {
            gyro1_data = bmi1.get_gyro_data();
        }

        if (bmi2.get_accel_status() & BMI08_ACCEL_DATA_READY_INT) {
            acc2_data = bmi2.get_accel_data();
        }
        if (bmi2.get_gyro_status() & BMI08_GYRO_DATA_READY_INT) {
            gyro2_data = bmi2.get_gyro_data();
        }
        
        if (adxl1.get_status() & ADXL375_DATA_READY) {
            adxl1_data = adxl1.get_data();
        }
        if (adxl2.get_status() & ADXL375_DATA_READY) {
            adxl2_data = adxl2.get_data();
        }

        // Populate NavSensors structure
        nav_sensors.bmi_accel = {acc1_data.x, acc1_data.y, acc1_data.z};
        nav_sensors.bmi_gyro = {gyro1_data.x, gyro1_data.y, gyro1_data.z}; // In DPS
        nav_sensors.bmi_aux_accel = {acc2_data.x, acc2_data.y, acc2_data.z};
        nav_sensors.bmi_aux_gyro = {gyro2_data.x, gyro2_data.y, gyro2_data.z}; // In DPS
        nav_sensors.adxl = {adxl1_data.x, adxl1_data.y, adxl1_data.z};
        nav_sensors.adxl_aux = {adxl2_data.x, adxl2_data.y, adxl2_data.z};

        // Set Kalman static mode for initial calibration
        if (current_ms < KALMAN_CALIB_DURATION_MS) {
            kalman.set_is_static(true);
        } else {
            if (!switched_to_non_static_printed) {
                 std::cout << "# Info: Switching to non-static mode for Kalman." << std::endl;
                 std::cout << "# Info: Initial Gyro Bias: " << kalman.get_gyro_bias().transpose() << std::endl;
                 std::cout << "# Info: Initial Accel Bias: " << kalman.get_accel_bias().transpose() << std::endl;
                 std::cout << "# Info: Initial Azimuth (deg): " << kalman.get_azimuth() * RAD_TO_DEG << std::endl;
                 std::cout << "# Info: Initial Pitch (deg): " << kalman.get_pitch() * RAD_TO_DEG << std::endl;
                 std::cout << "# Info: Initial Roll (deg): " << kalman.get_roll() * RAD_TO_DEG << std::endl;
                 switched_to_non_static_printed = true;
            }
            kalman.set_is_static(false);
        }

        // Run Kalman predict
        kalman.predict(nav_sensors, nav_data);

        // Get debug values
        Eigen::Vector3f fused_accel = kalman.get_fused_body_acceleration();
        Eigen::Vector3f c_bmi_val = kalman.get_debug_c_bmi();
        Eigen::Vector3f c_bmi_aux_val = kalman.get_debug_c_bmi_aux();

        // Print data: Time, Azimuth, Pitch, BMI1_ax, BMI1_ay, BMI1_az, BMI2_ax, BMI2_ay, BMI2_az
        // Fused_ax, Fused_ay, Fused_az, cBMI_x, cBMI_y, cBMI_z, cBMI_aux_x, cBMI_aux_y, cBMI_aux_z
        std::cout << current_ms << ","
                  << kalman.get_azimuth() * RAD_TO_DEG << ","
                  << kalman.get_pitch() * RAD_TO_DEG << ","
                  << nav_sensors.bmi_accel.x << ","
                  << nav_sensors.bmi_accel.y << ","
                  << nav_sensors.bmi_accel.z << ","
                  << nav_sensors.bmi_aux_accel.x << ","
                  << nav_sensors.bmi_aux_accel.y << ","
                  << nav_sensors.bmi_aux_accel.z << ","
                  << fused_accel(0) << "," << fused_accel(1) << "," << fused_accel(2) << ","
                  << c_bmi_val(0) << "," << c_bmi_val(1) << "," << c_bmi_val(2) << ","
                  << c_bmi_aux_val(0) << "," << c_bmi_aux_val(1) << "," << c_bmi_aux_val(2)
                  << std::endl;

        // No delay, run as fast as possible
    }

    std::cout << "Kalman predict test finished." << std::endl;

    return 0;
}
