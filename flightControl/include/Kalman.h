#ifndef KALMAN_H
#define KALMAN_H

#include "flightControl/eigen-3.4.0/Eigen/Dense"
#include "../data/sensors.h"
#include "../data/data.h"
#include "AvState.h"

#include <tuple>

struct Kalman_Rocket_State_;
typedef struct Kalman_Rocket_State_ Kalman_Rocket_State;

class KalmanFilter {

    private:
        Kalman_Rocket_State_* state;

    public:
        KalmanFilter();
        std::tuple <double, Vector3> UpdateAndGetAltitudeAndVelocity(Data data, AvState av_state);
        Vector3 GetAcceleration();

};


struct Kalman_Rocket_State_ {

    // tilde = prediction, hat = estimated

    // Covariance of state (Uncertainty associated with each element of X) => 6*6 matrices
    Eigen::Matrix<double, 10, 10> P_tilde; // Prediction
    Eigen::Matrix<double, 10, 10> P_hat; // Estimated covariance of the actual state

    // State : z, vx, vy, vz, ax, ay, az, wx, wy, wz
    // (altitude, velocity on x, velocity on y, velocity on z, acceleration on x, acceleration on y, acceleration on z,
    // angular velocity components)
    Eigen::Matrix<double, 10, 1> X_tilde;
    Eigen::Matrix<double, 10, 1> X_hat;

    // Variances measurement
    Eigen::Matrix<double, 1, 1> R_baro_bmp;
    Eigen::Matrix<double, 3, 3> R_acc_adxl;
    Eigen::Matrix<double, 3, 3> R_acc_bmi;
    Eigen::Matrix<double, 3, 3> R_gyro_bmi;

    // Process noise covariance matrix (which affects the system)
    Eigen::Matrix<double, 10, 10> Q;

    // System Dynamics
    Eigen::Matrix<double, 10, 10> F; // represents the dynamics of the system ; how the system's state evolves from
                                  // one time step to the next in the absence of control inputs

    Eigen::Matrix<double, 10, 6> G; // n_u = 6 (3 accelerations and 3 angular velocities)

    // Last time stamp
    uint32_t last_time;

    // Last temperature
    double last_temp_bmi;
    double last_temp_bmi_aux;

};


#endif