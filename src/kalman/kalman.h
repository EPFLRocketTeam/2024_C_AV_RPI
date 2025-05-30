#ifndef KALMAN_H
#define KALMAN_H

#include <Eigen/Dense>
#include "rotation_utils.h"
#include "data.h"

// conventions used : 
// The rotation matrix R that represents the orientation of the rocket is of the form
// R = (E N U).T
// where E is the east vector, N is the north vector and U is the up vector (in the rocket frame)
// This means that if v is a vector in the rocket frame, R * v is the same vector in the earth frame
// The quaternion q that represents the orientation of the rocket is of the form
// q = (scalar, vector)
// All units are in SI units

// Implementation of an ESKF (Error State Kalman Filter) for the rocket (multiplicative quaternion formulation)
// The state of the filter is a 15-dimensional vector that represents the error state
// state vector:
// [0:3] orientation error (rotation vector)
// [3:6] velocity error
// [6:9] position error
// [9:12] gyro bias error
// [12:15] accelerometer bias error
class Kalman {
private:
    Quaternion orientation_estimate;        // quaternion representing the orientation of the rocket
    Eigen::Vector3f velocity_estimate;      // velocity of the rocket in the earth frame
    Eigen::Vector3f position_estimate;      // position of the rocket in the earth frame
    Eigen::Vector3f gyro_bias;              // (estimated) bias of the gyroscope
    Eigen::Vector3f accelerometer_bias;     // (estimated) bias of the accelerometer
    Eigen::VectorXf state;                  // error state
    Eigen::MatrixXf estimate_covariance;    // covariance matrix of the error state
    Eigen::MatrixXf initial_estimate_covariance; // initial covariance matrix of the error state (used for callibration reset)
    Eigen::Matrix3f observation_covariance; // covariance matrix of the observation noise
    Eigen::Matrix3f gyro_cov_mat;           // covariance matrix of the gyroscope noise
    Eigen::Matrix3f gyro_bias_cov_mat;      // covariance matrix of the gyroscope bias noise
    Eigen::Matrix3f accel_cov_mat;          // covariance matrix of the accelerometer noise
    Eigen::Matrix3f accel_bias_cov_mat;     // covariance matrix of the accelerometer bias noise
    Eigen::MatrixXf G;                      // Jacobian of the error state dynamics
    float initial_azimuth;
    float g;                                // gravity norm
    Quaternion orientation_dot;             // quaternion time derivative
    unsigned long last_measurement_time = 0; // time of the last IMU measurement (in milliseconds)
    float time_delta = 0;                   // time between current and previous IMU measurements (in seconds)

    Vector3 current_accel = {0, 0, 0};

    // static periodic calibration
    bool is_static = true;
    bool is_calibrating = false;
    Eigen::Vector3f gyro_static_sum = Eigen::Vector3f::Zero();
    Eigen::Vector3f accel_static_sum = Eigen::Vector3f::Zero();
    Eigen::Vector3f accel_static_avg = Eigen::Vector3f::Zero();
    int static_counter = 0;
    unsigned long static_calib_start_time = 0;
    unsigned long last_static_calib_time = 0;
    const unsigned long static_calib_duration = 5000; // 5 second
    const unsigned long static_recalibration_interval = 120000; // 2 minutes between two static calibrations

    // Initial position (to set the coordinates origin)
    double initial_lon = 0;
    double initial_lat = 0;
    double initial_alt = 0;


    // Last gps observation (to check if we should update)
    double last_gps_lat = 0;
    double last_gps_lon = 0;

public:
    Kalman( float estimate_covariance_gyro, 
            float estimate_covariance_accel,
            float estimate_covariance_orientation,
            float gyro_cov, 
            float gyro_bias_cov,
            float accel_proc_cov, 
            float accel_bias_cov,
            float gps_obs_cov, 
            float alt_obs_cov);
    ~Kalman() = default;

    void calculate_initial_orientation();
    Eigen::MatrixXf process_covariance();

    void check_static(const DataDump& dump);

    // takes the NavSensors data and outputs a single fused IMU measurement for all IMUs
    void fuse_IMUs(const NavSensors& nav_sensors, Eigen::Vector3f& output_gyro_meas, Eigen::Vector3f& output_acc_meas);

    // converts the GPS and pressure measurements to the observation space (in meters)
    void gps_pressure_to_obs(const NavSensors& nav_sensors, const NavigationData& nav_data, Eigen::Vector3f& gps_meas, float& alt_meas);

    // void predict(const Eigen::Vector3f& gyro_meas, const Eigen::Vector3f& acc_meas);
    void predict(const NavSensors& nav_sensors, const NavigationData& nav_data);
    void update(const NavSensors& nav_sensors, const NavigationData& nav_data);

    Quaternion get_orientation() const;
    Eigen::Vector3f get_velocity() const;
    Eigen::Vector3f get_position() const;
    Eigen::Vector3f get_gyro_bias() const;
    Eigen::Vector3f get_accel_bias() const;
    float get_azimuth() const;
    float get_pitch() const;
    float get_roll() const;
    NavigationData get_nav_data() const;

};

#endif // KALMAN_H