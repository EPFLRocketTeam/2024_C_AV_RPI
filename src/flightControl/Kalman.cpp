#include <stdio.h>
#include <math.h>
#include <iostream>
#include <tuple>

// Put Eigen (extracted zip file) in a directory where you put your existing working header file
#include "flightControl/eigen-3.4.0/Eigen/Dense"
#include "flightControl/eigen-3.4.0/unsupported/Eigen/MatrixFunctions"
#include "flightControl/AvData.h"
#include "flightControl/Kalman.h"

// X_State_Vector : z, v, a, p0, k, h0
// altitude (m), velocity (m/s), acceleration (m/s^2), initial pressure (Pa), scala factor change altitude-pressure, 
// initial altitude (m)

// Constants

#define g       9.81f // standard gravity (m/s²)
#define M       0.02897f // air's molar mass (kg/mol)
#define R       8.3145f // ideal gas constant (J/(mol*K))
#define T       298.15f // reference temperature (K)

// Variances

#define sigma_z_baro    5.0f // for Pa
#define sigma_z_acc     25.0f // for m

#define sigma_a     1.0f // for m/s²
#define sigma_p0    0.1f // for Pa
#define sigma_k     1e-9f // for 

// alt0 : reference altitude for the barometer and its pressure
void kalman_setup(Kalman_Rocket_State * state, float alt0, float p0) {

    //state->X_tilde << alt0, 0.0f, 0.0f, p0, M/(R*T), alt0; // M/(R*T) or -M/(R*T)
    state->X_tilde << alt0, 0.0f, 0.0f, p0, -M/(R*T), alt0;

    state->X_hat << state->X_tilde;
    /*
   state->P_tilde.diagonal() << 25.0, 0.25, 0.25, 25.0, 1e-12, 25.0;
    */

   state->P_tilde << 25.0, 0, 0, 0, 0, 0,  // Assume high initial uncertainty in altitude
                   0, 0.25, 0, 0, 0, 0,  // and lower in others
                   0, 0, 0.25, 0, 0, 0,  // High uncertainty in acceleration
                   0, 0, 0, 25.0, 0, 0, // Initial pressure uncertainty
                   0, 0, 0, 0, 1e-12, 0,   // Small uncertainty in k
                   0, 0, 0, 0, 0, 25.0;  // Initial altitude uncertainty
    
    state->P_hat << state->P_tilde;
    
    state->R_baro << powf(sigma_z_baro, 2.0f);

    state->R_acc << powf(sigma_z_acc, 2.0f);

    //state->Q.diagonal() << powf(sigma_a, 2.0f),  powf(sigma_p0, 2.0f), powf(sigma_k, 2.0f);
   
   state->Q << powf(sigma_a, 2.0f), 0, 0,  // Process noise for altitude, velocity, and acceleration
                0, powf(sigma_p0, 2.0f), 0,
                0, 0, powf(sigma_k, 2.0f);

    // will be overwritten?
    state->F <<     0,  1,  0,  0,  0,  0,
                    0,  0,  1,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0;

    
    state->G << 0,  0,  0, // Minimal direct noise impact on altitude
                0,  0,  0, // Minimal direct noise impact on velocity
                1,  0,  0, // Direct noise impact on acceleration
                0,  1,  0, // Some noise impact on initial pressure (if modeled)
                0,  0,  1, // Example: Some noise impact on other state variables
                0,  0,  0; // Minimal direct noise impact on initial altitude (if modeled)

    state->last_time = 0;
}

void kalman_predict(Kalman_Rocket_State * state, float dt) {

    /* LEUR VERSION 
    //Create the discrete matrix
    Eigen::Matrix<float, 12, 12> A;

    A << -state->F, state->G*state->Q*state->G.transpose(), Eigen::MatrixXf::Zero(6, 6), state->F.transpose();
    A << A*dt;

    Eigen::Matrix<float, 12, 12> B;
    B << A.exp();

    Eigen::Matrix<float, 6, 6> PHI;
    PHI << B.block<6, 6>(6, 6).transpose();

    Eigen::Matrix<float, 6, 6> Q_w;
    Q_w << PHI*B.block<6, 6>(0, 6);

    //Compute step
    state->X_tilde << PHI*state->X_hat;

    state->P_tilde << PHI*state->P_hat*PHI.transpose() + Q_w;
    */

   // Define the state transition matrix F for the given system
    // Update F to include the effect of acceleration on velocity and altitude
    state->F << 1, dt, 0.5 * dt * dt, 0, 0, 0,  // Altitude updated with velocity and acceleration
                0, 1, dt, 0, 0, 0,              // Velocity updated with acceleration
                0, 0, 1, 0, 0, 0,              // Acceleration assumed constant over small dt
                0, 0, 0, 1, 0, 0,              // p0, k, h0 assumed constant over small dt
                0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 1;

    // Predict the next state vector
    state->X_tilde = state->F * state->X_hat;

    // Adjust Q to be compatible with the size of F and the state vector
    // If Q was originally designed for acceleration-related process noise, then
    // it needs to be integrated into a larger Q that covers the whole state vector.
    Eigen::Matrix<float, 6, 6> Q_expanded = Eigen::Matrix<float, 6, 6>::Zero();
    Q_expanded(2, 2) = state->Q(0, 0); // Assuming acceleration-related process noise in original Q

    // Predict the next state covariance matrix with expanded Q integrated
    state->P_tilde = state->F * state->P_hat * state->F.transpose() + Q_expanded;
}

void kalman_update_baro(Kalman_Rocket_State * state, float p) {

    /*
    float h = state->X_tilde(0, 0);
    float p0 = state->X_tilde(3, 0);
    float k = state->X_tilde(4, 0);
    float h0 = state->X_tilde(5, 0);

    // Expected pressure at the current estimated altitude (barometric formula)
    float p_est = p0 * expf(k * g * (h0-h));

    // Jacobian
    Eigen::Matrix<float, 1, 6> H;
    H << -k*g*p_est, 0.0, 0.0, p_est/p0, g*(h0-h)*p_est, k*g*p_est;

    // Kalman Gain
    Eigen::Matrix<float, 6, 1> K;
    K << state->P_tilde*H.transpose()*(H*state->P_tilde*H.transpose() + state->R_baro).inverse();

    // Update state estimate
    state->X_hat << state->X_tilde + K*(p - p_est);

    // Update error covariance matrix
    state->P_hat << (Eigen::MatrixXf::Identity(6,6) - K*H)*state->P_tilde;
    */

   /*
   Eigen::Matrix<float, 1, 6> H;
    H << 1, 0, 0, 0, 0, 0;  // Measurement matrix for altitude
    float z_hat = p;  // Convert p to altitude if necessary
    Eigen::Matrix<float, 1, 1> Y;
    Y << z_hat - (H * state->X_tilde)(0, 0);

    // Calculation of S should result in a 1x1 matrix
    Eigen::Matrix<float, 1, 1> S = H * state->P_tilde * H.transpose() + state->R_baro;

    // Kalman Gain calculation, K should be 6x1
    Eigen::Matrix<float, 6, 1> K = state->P_tilde * H.transpose() * S.inverse();

    // Update state estimate
    state->X_hat += K * Y;

    // Update error covariance matrix
    state->P_hat -= K * H * state->P_tilde;
    */

   Eigen::Matrix<float, 1, 6> H;
    H << 1, 0, 0, 0, 0, 0;  // Measurement matrix for altitude

    Eigen::Matrix<float, 1, 1> Z;
    Z << p;  // The altitude measurement converted from pressure

    Eigen::Matrix<float, 1, 1> Y;
    Y << Z - H * state->X_tilde;  // Z is a 1x1 matrix, H * state->X_tilde results in a 1x1 matrix

    Eigen::Matrix<float, 1, 1> S = H * state->P_tilde * H.transpose() + state->R_baro;  // Residual covariance

    Eigen::Matrix<float, 6, 1> K = state->P_tilde * H.transpose() * S.inverse();  // Kalman Gain

    state->X_hat = state->X_tilde + K * Y;  // Update state estimate

    state->P_hat = (Eigen::Matrix<float, 6, 6>::Identity() - K * H) * state->P_tilde;  // Update error covariance matrix
}

void kalman_update_acc(Kalman_Rocket_State * state, float a) {

    /*
    Eigen::Matrix<float, 1, 6> H;
    H << 0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
    
    Eigen::Matrix<float, 6, 1> K;
    K << state-> P_tilde * H.transpose() * (H * state->P_tilde * H.transpose() + state->R_acc).inverse();

    state->X_hat << state->X_tilde + K * (a - state->X_tilde(2)); // if (0) like their version: nan

    state->P_hat << (Eigen::MatrixXf::Identity(6,6) - K * H) * state->P_tilde;
    */

   // H matrix maps the state vector's acceleration component to the measured acceleration.
    Eigen::Matrix<float, 1, 6> H;
    H << 0, 0, 1, 0, 0, 0;  // Only the acceleration affects the measurement directly.

    // Y is the measurement residual: the difference between the measured acceleration
    // and the predicted acceleration from the state vector.
    Eigen::Matrix<float, 1, 1> Y;
    Y << a - (H * state->X_tilde)(0, 0);  // Assuming 'a' is the measured acceleration.

    // S is the residual covariance, incorporating both the measurement noise and the
    // uncertainty of the predicted state as it relates to the measurement.
    Eigen::Matrix<float, 1, 1> S = H * state->P_tilde * H.transpose() + state->R_acc;

    // K is the Kalman Gain, determining how much the measurement should influence the state update.
    Eigen::Matrix<float, 6, 1> K = state->P_tilde * H.transpose() * S.inverse();

    // Update the state estimate using the measurement residual, scaled by the Kalman gain.
    state->X_hat = state->X_tilde + K * Y;

    // Update the estimate's covariance matrix, reflecting the reduced uncertainty.
    state->P_hat = (Eigen::Matrix<float, 6, 6>::Identity() - K * H) * state->P_tilde;

}

#include <cmath>

// Constants for the International Standard Atmosphere
const float P0 = 101325.0f;  // Sea level standard atmospheric pressure, in Pascals
const float T0 = 288.15f;    // Sea level standard temperature, in Kelvins
const float L = 0.0065f;     // Temperature lapse rate, in K/m

/**
 * Converts barometric pressure readings to altitude using the International Standard Atmosphere model.
 * @param pressure The pressure in Pascals.
 * @return The altitude above sea level in meters.
 */
float pressure_to_altitude(float pressure) {
    return (1.0f - powf(pressure / P0, R * L / (g * M))) * T0 / L;
}

void kalman_handle_data(Kalman_Rocket_State * state, AvData data) {

    /*
    // To handle
    if (state == NULL) {
        std::cout << "state is NULL => return" << std::endl;
        return;
    }

    float dt = (data.time - state->last_time)/1000.0f; // ATTENTION CONVERSION DE MS EN SECONDES PEUT ETRE NON NECESSAIRE
    state->last_time = data.time;
    kalman_predict(state, dt);

    kalman_update_baro(state, data.pressure);
    kalman_update_acc(state, data.acceleration);
    */

   // Calculate the time elapsed since the last update in seconds
    float dt = (data.time - state->last_time) / 1000.0f; // Conversion from milliseconds to seconds
    //kalman_predict(state, dt);
    if (dt > 0 && data.ignited) {
        // If time has passed and the rocket is ignited, predict the next state
        kalman_predict(state, dt);
        state->last_time = data.time; // Update the last processed time
        
        // If accelerometer data is available, update acceleration
        kalman_update_acc(state, data.acceleration);
        
        // If barometer data is available, convert it to altitude and update
        if (data.pressure > 0) {
            float altitude = pressure_to_altitude(data.pressure); // Convert pressure to altitude
            kalman_update_baro(state, altitude); // Update Kalman Filter with the altitude
        }
    }
    state->last_time = data.time;

}

Kalman_Rocket_State* kalman_entry() {

    // TO BE MODIFIED
    static float p0 = 100500.710127303867f;
    static float alt0 = 165.0f;

    //static uint8_t first_time = 10;
    //static float p0 = 92000.0f;
    //static float alt0 = 120.0f;

    Kalman_Rocket_State * state = new Kalman_Rocket_State;
    
    //  in C++, the new operator throws a std::bad_alloc exception if it fails to allocate memory, rather than returning NULL.
    if (state == NULL) {
        std::cerr << "Error: Couldn't create a Kalman_Rocket_State in kalman_entry()" << "\n" << std::endl;
        return NULL;
    }

    kalman_setup(state, alt0, p0);

    return state;
}

// Defining KalmanFilter

KalmanFilter::KalmanFilter() {
    std::cout << "In new constructor" << "\n" << std::endl;
    state = kalman_entry();

    if (state == NULL) {
        std::cerr << "Error: KalmanFilter contains a NULL Kalman_Rocket_State" << "\n" << std::endl;
    }
}

std::tuple<float, float> KalmanFilter::UpdateAndGetAltitudeAndVelocity(AvData data) {

    kalman_handle_data(state, data);
    return std::make_tuple(state->X_hat(0,0), state->X_hat(1,0)); // returns the estimated altitude and velocity
}
