#include <iostream>
#include <iomanip> // For std::fixed and std::setprecision
#include <unistd.h> // For usleep
#include <chrono>   // For millis/micros

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

    std::cout << "Calibrating ADXL375 sensors..." << std::endl;
    adxl1.calibrate();
    adxl2.calibrate();
    std::cout << "ADXL375 calibration done." << std::endl;

    // Kalman Initialization with example parameters
    Kalman kalman(
        0.1f,    // estimate_covariance_gyro
        0.1f,    // estimate_covariance_accel
        0.01f,   // estimate_covariance_orientation
        0.001f,  // gyro_cov (process noise for gyro)
        0.0001f, // gyro_bias_cov (process noise for gyro bias)
        0.01f,   // accel_proc_cov (process noise for accel)
        0.001f,  // accel_bias_cov (process noise for accel bias)
        1.0f,    // gps_obs_cov (not used in this test)
        1.0f     // alt_obs_cov (not used in this test)
    );

    NavSensors nav_sensors;
    NavigationData nav_data; // Will be mostly empty/default for this predict-only test

    // Set dummy GPS and pressure data for Kalman's initial calibration if it uses them
    nav_data.position.lat = 0.0;
    nav_data.position.lng = 0.0;
    // Standard sea-level pressure in Pa, then converted to hPa for BMP data struct
    nav_sensors.bmp.pressure = 1013.25f; 
    nav_sensors.bmp_aux.pressure = 1013.25f;


    const int LOOP_DELAY_US = 10000; // 10ms loop time (100 Hz)
    const uint32_t KALMAN_CALIB_DURATION_MS = 5000; // 5 seconds for Kalman static calibration
    const int TOTAL_DURATION_S = 20; // Run for 20 seconds
    const int TOTAL_ITERATIONS = (TOTAL_DURATION_S * 1000000) / LOOP_DELAY_US;

    std::cout << "Starting Kalman predict test for " << TOTAL_DURATION_S << " seconds..." << std::endl;
    std::cout << "Kalman static calibration will run for the first " << KALMAN_CALIB_DURATION_MS / 1000 << " seconds." << std::endl;

    // Print CSV header
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Time_ms,Azimuth_deg,Pitch_deg,BMI1_ax,BMI1_ay,BMI1_az,BMI2_ax,BMI2_ay,BMI2_az" << std::endl;

    // Initialize Kalman's last_measurement_time by calling micros once before the loop
    // This is handled internally by Kalman's first predict call where last_measurement_time is 0.

    for (int i = 0; i < TOTAL_ITERATIONS; ++i) {
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
            if (kalman.get_gyro_bias().norm() == 0 && kalman.get_accel_bias().norm() == 0 && current_ms >= KALMAN_CALIB_DURATION_MS && i > 0 && (millis() - KALMAN_CALIB_DURATION_MS < 2* (LOOP_DELAY_US/1000) ) ) { // Check if it just switched
                 std::cout << "# Info: Switching to non-static mode for Kalman." << std::endl;
                 std::cout << "# Info: Initial Gyro Bias: " << kalman.get_gyro_bias().transpose() << std::endl;
                 std::cout << "# Info: Initial Accel Bias: " << kalman.get_accel_bias().transpose() << std::endl;
                 std::cout << "# Info: Initial Azimuth (deg): " << kalman.get_azimuth() * RAD_TO_DEG << std::endl;
                 std::cout << "# Info: Initial Pitch (deg): " << kalman.get_pitch() * RAD_TO_DEG << std::endl;
                 std::cout << "# Info: Initial Roll (deg): " << kalman.get_roll() * RAD_TO_DEG << std::endl;
            }
            kalman.set_is_static(false);
        }

        // Run Kalman predict
        kalman.predict(nav_sensors, nav_data);

        // Print data: Time, Azimuth, Pitch, BMI1_ax, BMI1_ay, BMI1_az, BMI2_ax, BMI2_ay, BMI2_az
        std::cout << current_ms << ","
                  << kalman.get_azimuth() * RAD_TO_DEG << ","
                  << kalman.get_pitch() * RAD_TO_DEG << ","
                  << nav_sensors.bmi_accel.x << ","
                  << nav_sensors.bmi_accel.y << ","
                  << nav_sensors.bmi_accel.z << ","
                  << nav_sensors.bmi_aux_accel.x << ","
                  << nav_sensors.bmi_aux_accel.y << ","
                  << nav_sensors.bmi_aux_accel.z
                  << std::endl;

        usleep(LOOP_DELAY_US);
    }

    std::cout << "Kalman predict test finished." << std::endl;

    return 0;
}
