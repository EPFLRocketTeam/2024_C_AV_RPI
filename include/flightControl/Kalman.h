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
        std::tuple <float, float> UpdateAndGetAltitudeAndVelocity(AvData data);

};


struct Kalman_Rocket_State_ {

    // tilde = prediction, hat = estimated

    // Covariance of state (Uncertainty associated with each element of X) => 6*6 matrices
    Eigen::Matrix<float, 6, 6> P_tilde; // Prediction
    Eigen::Matrix<float, 6, 6> P_hat; // Estimated covariance of the actual state

    // State : z, v, a, p0, k, h0 (altitude, velocity, acceleration, initial pressure, , initial altitude)
    Eigen::Matrix<float, 6, 1> X_tilde;
    Eigen::Matrix<float, 6, 1> X_hat;

    // Variances measurement
    Eigen::Matrix<float, 1, 1> R_baro;
    Eigen::Matrix<float, 1, 1> R_acc;

    // Process noise covariance matrix (which affects the system)
    Eigen::Matrix<float, 3, 3> Q;

    // System Dynamics
    Eigen::Matrix<float, 6, 6> F; // represents the dynamics of the system ; how the system's state evolves from
                                  // one time step to the next in the absence of control inputs

    Eigen::Matrix<float, 6, 3> G; // represents the effect of control inputs on the system dynamics, if any

    // Last time stamp
    unsigned int last_time;

};


#endif