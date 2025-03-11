#ifndef KALMAN_H
#define KALMAN_H

#include <Eigen/Dense>
#include "rotation_utils.h"
#include "h_driver.h"

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
class Kalman : public HDriver {
private:
    Quaternion orientation_estimate;        // quaternion representing the orientation of the rocket
    Eigen::Vector3f velocity_estimate;      // velocity of the rocket in the earth frame
    Eigen::Vector3f position_estimate;      // position of the rocket in the earth frame
    Eigen::Vector3f gyro_bias;              // (estimated) bias of the gyroscope
    Eigen::Vector3f accelerometer_bias;     // (estimated) bias of the accelerometer
    Eigen::VectorXf state;                  // error state
    Eigen::MatrixXf estimate_covariance;    // covariance matrix of the error state
    Eigen::Matrix3f observation_covariance; // covariance matrix of the observation noise
    Eigen::Matrix3f gyro_cov_mat;           // covariance matrix of the gyroscope noise
    Eigen::Matrix3f gyro_bias_cov_mat;      // covariance matrix of the gyroscope bias noise
    Eigen::Matrix3f accel_cov_mat;          // covariance matrix of the accelerometer noise
    Eigen::Matrix3f accel_bias_cov_mat;     // covariance matrix of the accelerometer bias noise
    Eigen::MatrixXf G;                      // Jacobian of the error state dynamics
    float g;                                // gravity norm
    Quaternion orientation_dot;             // quaternion time derivative
    unsigned long last_measurement_time = 0; // time of the last IMU measurement (in milliseconds)
    float time_delta = 0;                   // time between current and previous IMU measurements (in seconds)
    float initial_azimuth = 0;              // initial azimuth angle

    // static continuous calibration
    bool is_calibrating = false;
    Eigen::Vector3f gyro_static_sum = Eigen::Vector3f::Zero();
    Eigen::Vector3f accel_static_sum = Eigen::Vector3f::Zero();
    Eigen::Vector3f accel_static_avg = Eigen::Vector3f::Zero();
    int static_counter = 0;
    unsigned long static_calib_start_time = 0;
    unsigned long last_static_calib_time = 0;
    const unsigned long static_calib_duration = 5000; // 5 second
    const unsigned long static_recalibration_interval = 120000; // 2 minutes between two static calibrations

    // Helper method for skew-symmetric matrix creation
    Eigen::Matrix3f skew_symmetric_eigen(const Eigen::Vector3f& v);

public:
    Kalman(const Quaternion& initial_orientation_est, 
           const Eigen::Vector3f& initial_gyro_bias_est, 
           const Eigen::Vector3f& initial_accel_bias_est,
           float estimate_covariance_val, 
           float gyro_cov, 
           float gyro_bias_cov,
           float accel_proc_cov, 
           float accel_bias_cov,
           float gps_obs_cov, 
           float alt_obs_cov,
           float initial_azimuth);

    void calculate_initial_orientation();
    Eigen::MatrixXf process_covariance();
    void predict(const Eigen::Vector3f& gyro_meas, const Eigen::Vector3f& acc_meas, bool is_static = false);
    void update(const Eigen::Vector3f& gps_meas, float alt_meas, bool is_static = false);

    Quaternion get_orientation() const;
    Eigen::Vector3f get_velocity() const;
    Eigen::Vector3f get_position() const;
    Eigen::Vector3f get_gyro_bias() const;
    Eigen::Vector3f get_accel_bias() const;
};

#endif // KALMAN_H