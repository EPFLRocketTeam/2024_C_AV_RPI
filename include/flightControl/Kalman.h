#ifndef KALMAN_H
#define KALMAN_H

#include "flightControl/AvData.h"
#include "flightControl/eigen-3.4.0/Eigen/Dense"

#include <tuple>

struct Kalman_Rocket_State_;
typedef struct Kalman_Rocket_State_ Kalman_Rocket_State;

class KalmanFilter {

    private:
        Kalman_Rocket_State_* state;

    public:
        KalmanFilter();
        std::tuple <float, float> UpdateAndGetAltitudeAndVelocity(State, Data, AvState);

};


struct Kalman_Rocket_State_ {

    // tilde = prediction, hat = estimated

    // Covariance of state (Uncertainty associated with each element of X) => 6*6 matrices
    Eigen::Matrix<float, 9, 9> P_tilde; // Prediction
    Eigen::Matrix<float, 9, 9> P_hat; // Estimated covariance of the actual state

    // State : z, v, a, p0, k, h0, wx, wy, wz
    // (altitude, velocity, acceleration, initial pressure, scale factor change altitude-pressure, 
    // initial altitude, angular velocity components)

    Eigen::Matrix<float, 9, 1> X_tilde;
    Eigen::Matrix<float, 9, 1> X_hat;

    // Variances measurement
    Eigen::Matrix<float, 1, 1> R_baro_bmp;
    Eigen::Matrix<float, 1, 1> R_acc_adxl;
    Eigen::Matrix<float, 1, 1> R_acc_bmi;
    Eigen::Matrix<float, 3, 3> R_gyro_bmi;

    // Process noise covariance matrix (which affects the system)
    Eigen::Matrix<float, 6, 6> Q;

    // System Dynamics
    Eigen::Matrix<float, 9, 9> F; // represents the dynamics of the system ; how the system's state evolves from
                                  // one time step to the next in the absence of control inputs

    Eigen::Matrix<float, 9, 6> G; // represents the effect of control inputs on the system dynamics, if any

    // Last time stamp
    uint32_t last_time;

};


#endif