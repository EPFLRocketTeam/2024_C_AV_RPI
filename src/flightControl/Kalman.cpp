#include <stdio.h>
#include <math.h>
#include <cmath> // for calculating pressure based on altitude (useful?)
#include <iostream>
#include <tuple>

// Put Eigen (extracted zip file) in a directory where you put your existing working header file
#include "flightControl/eigen-3.4.0/Eigen/Dense"
#include "flightControl/eigen-3.4.0/unsupported/Eigen/MatrixFunctions"
#include "flightControl/AvData.h"
#include "flightControl/AvState.h"
#include "flightControl/Kalman.h"
#include "data/sensors.h"
#include "data/data.h"

// State : z, v, a, p0, k, h0, wx, wy, wz
// (altitude, velocity, acceleration, initial pressure, scale factor change altitude-pressure, 
// initial altitude, angular velocity components)

// Constants

#define g       9.81f // standard gravity (m/s²)
#define M       0.02897f // air's molar mass (kg/mol)
#define R       8.3145f // ideal gas constant (J/(mol*K))
#define T       298.15f // reference temperature (K)

// Variances
// TODO: Determine variances for sensors and initial uncertainties

#define sigma_z_baro_bmp    5.0f // for Pa
#define sigma_z_acc_adxl   25.0f // for m
#define sigma_z_acc_bmi    25.0f // for m
#define sigma_z_gyro_x_bmi  1.0f // todo
#define sigma_z_gyro_y_bmi  1.0f // todo
#define sigma_z_gyro_z_bmi  1.0f // todo


// Used to initialize Q
// Helps KF to appropriately incorporate new acceleration data while balancing it 
// with the existing state estimate and its uncertainty
#define sigma_a     1.0f // for m/s²
#define sigma_p0    0.1f // for Pa
#define sigma_k     1e-9f // for 
#define sigma_wx    1.0f // for rad/s // todo
#define sigma_wy    1.0f // for rad/s // todo
#define sigma_wz    1.0f // for rad/s // todo

// - set to low values if altitude and velocity are primarily driven by your measurements and 
//   system model rather than by unmodeled process noise
// - to adjust based on simulations
#define sigma_z  0.0f
#define sigma_v  0.0f
#define sigma_h0 0.0f


// alt0: reference altitude for the barometer 
// p0: pressure at alt0
void kalman_setup(Kalman_Rocket_State * state, float alt0, float p0, float wx0, float wy0, float wz0) {

    state->X_tilde << alt0, 0.0f, 0.0f, p0, -M/(R*T), alt0, wx0, wy0, wz0;

    state->X_hat << state->X_tilde;
    
    // TODO: initial uncertainties to be determined for angular velocities
    state->P_tilde.diagonal() << 25.0, 0.25, 0.25, 25.0, 1e-12, 25.0, 1.0, 1.0, 1.0;
    
    /*
   state->P_tilde << 25.0, 0, 0, 0, 0, 0,  // Assume high initial uncertainty in altitude
                   0, 0.25, 0, 0, 0, 0,  // and lower in others
                   0, 0, 0.25, 0, 0, 0,  // High uncertainty in acceleration
                   0, 0, 0, 25.0, 0, 0, // Initial pressure uncertainty
                   0, 0, 0, 0, 1e-12, 0,   // Small uncertainty in k
                   0, 0, 0, 0, 0, 25.0;  // Initial altitude uncertainty
    */
    
    state->P_hat << state->P_tilde;
    
    state->R_baro_bmp << powf(sigma_z_baro_bmp, 2.0f);
    state->R_acc_adxl << powf(sigma_z_acc_adxl, 2.0f);
    state->R_acc_bmi << powf(sigma_z_acc_bmi, 2.0f);
    // TODO: Determine if noises are independent (if not, R_gyro is not diagonal)
    state->R_gyro_bmi.diagonal() << powf(sigma_z_gyro_x_bmi, 2.0f), powf(sigma_z_gyro_y_bmi, 2.0f), powf(sigma_z_gyro_z_bmi, 2.0f);

    // Process noise for altitude, velocity, and acceleration (and the rest)
    state->Q.diagonal() << powf(sigma_z, 2.0f), powf(sigma_v, 2.0f), powf(sigma_a, 2.0f),
                            powf(sigma_p0, 2.0f), powf(sigma_k, 2.0f), powf(sigma_h0, 2.0f),
                            powf(sigma_wx, 2.0f), powf(sigma_wy, 2.0f), powf(sigma_wz, 2.0f);

    // no need to setup F (done at each step in kalman_predict)
    state->F.setZero(9, 9);
    
    //TODO: Verify G initialization
    state->G << 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, // Acceleration controlled
                0, 1, 0, 0, 0, 0, // Initial pressure affected (?)
                0, 0, 1, 0, 0, 0, // Scaled factor affected (?)
                0, 0, 0, 0, 0, 0, // Initial altitude unaffected
                0, 0, 0, 1, 0, 0, // Angular velocity wx controlled
                0, 0, 0, 0, 1, 0, // Angular velocity wy controlled
                0, 0, 0, 0, 0, 1; // Angular velocity wz controlled

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

    /* MY LINEAR VERSION */
    // Define the state transition matrix F for the given system
    // Update F to include the effect of acceleration on velocity and altitude
    state->F << 1, dt, 0.5 * dt * dt, 0, 0, 0, 0, 0, 0, // altitude (z)
                0, 1, dt, 0, 0, 0, 0, 0, 0,             // velocity (v)
                0, 0, 1, 0, 0, 0, 0, 0, 0,              // acceleration (a)
                0, 0, 0, 1, 0, 0, 0, 0, 0,              // initial pressure (p0)
                0, 0, 0, 0, 1, 0, 0, 0, 0,              // scale factor change (k)
                0, 0, 0, 0, 0, 1, 0, 0, 0,              // initial altitude (h0)
                0, 0, 0, 0, 0, 0, 1, 0, 0,              // angular velocity (wx)
                0, 0, 0, 0, 0, 0, 0, 1, 0,              // angular velocity (wy)
                0, 0, 0, 0, 0, 0, 0, 0, 1;              // angular velocity (wz)


    float wx = state->X_tilde(6);
    float wy = state->X_tilde(7);
    float wz = state->X_tilde(8);

    // Simple rotation effect based on angular velocities
    Eigen::Matrix<float, 3, 3> rotation_effect;
    rotation_effect << 0, -wz * dt, wy * dt,
                       wz * dt, 0, -wx * dt,
                       -wy * dt, wx * dt, 0;

    // Apply the rotation effect to the acceleration components in the state vector
    Eigen::Matrix<float, 9, 1> temp_accel = state->X_tilde.block<3, 1>(2, 0) + rotation_effect * state->X_tilde.block<3, 1>(2, 0);

    // Update the acceleration in the state vector
    state->X_tilde.block<3, 1>(2, 0) = temp_accel;

    // Predict the next state vector
    state->X_tilde = state->F * state->X_hat;
    
    state->P_tilde = state->F * state->P_hat * state->F.transpose() + state->G * state->Q * state->G.transpose();
    
    
}

// Converts barometric pressure readings to altitude using the International Standard Atmosphere model
// pressure in Pascals ; temperature in Kelvins
// Returns The altitude above sea level in meters
float pressure_to_altitude(float pressure, float temperature) {

    // Constants for the International Standard Atmosphere
    static const float P0 = 101325.0f;  // Sea level standard atmospheric pressure (in Pascals)
    static const float T0 = 288.15f;    // Sea level standard temperature (in Kelvins)
    static const float L = 0.0065f;     // Temperature lapse rate (in K/m)

    return (1.0f - powf(pressure / P0, R * L / (g * M))) * temperature / L;
}

void kalman_update_baro(Kalman_Rocket_State * state, float p, float temperature, Eigen::Matrix<float, 1, 1> R_baro) {

    /* LEUR VERSION
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

   /* MY LINEAR VERSION */
   Eigen::Matrix<float, 1, 9> H; // Measurement matrix for altitude
    H.setZero();
    H(0, 0) = 1;  // Altitude is directly measured

    Eigen::Matrix<float, 1, 1> Z;
    Z << pressure_to_altitude(p, temperature);

    Eigen::Matrix<float, 1, 1> Y;
    Y << Z - H * state->X_tilde;  // Z is a 1x1 matrix, H * state->X_tilde results in a 1x1 matrix

    Eigen::Matrix<float, 1, 1> S = H * state->P_tilde * H.transpose() + R_baro;  // Residual covariance

    Eigen::Matrix<float, 9, 1> K = state->P_tilde * H.transpose() * S.inverse();  // Kalman Gain

    state->X_hat = state->X_tilde + K * Y;  // Update state estimate

    state->P_hat = (Eigen::Matrix<float, 9, 9>::Identity() - K * H) * state->P_tilde;  // Update error covariance matrix
    
    
}

void kalman_update_acc(Kalman_Rocket_State * state, float a, Eigen::Matrix<float, 1, 1> R_acc) {

    /* LEUR VERSION
    Eigen::Matrix<float, 1, 6> H;
    H << 0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
    
    Eigen::Matrix<float, 6, 1> K;
    K << state-> P_tilde * H.transpose() * (H * state->P_tilde * H.transpose() + state->R_acc).inverse();

    state->X_hat << state->X_tilde + K * (a - state->X_tilde(2)); // if (0) like their version: nan

    state->P_hat << (Eigen::MatrixXf::Identity(6,6) - K * H) * state->P_tilde;
    */

   /* MY LINEAR VERSION */
   // H matrix maps the state vector's acceleration component to the measured acceleration
    Eigen::Matrix<float, 1, 9> H;
    H.setZero();
    H(0, 2) = 1;  // Only the acceleration affects the measurement directly

    // Y is the measurement residual: the difference between the measured acceleration
    // and the predicted acceleration from the state vector
    Eigen::Matrix<float, 1, 1> Y;
    Y << a - (H * state->X_tilde)(0, 2);  // TODO: not sure if (0,0) or (0,2)

    // S is the residual covariance, incorporating both the measurement noise and the
    // uncertainty of the predicted state as it relates to the measurement
    Eigen::Matrix<float, 1, 1> S = H * state->P_tilde * H.transpose() + R_acc;

    // K is the Kalman Gain, determining how much the measurement should influence the state update
    Eigen::Matrix<float, 6, 1> K = state->P_tilde * H.transpose() * S.inverse();

    // Update the state estimate using the measurement residual, scaled by the Kalman gain
    state->X_hat = state->X_tilde + K * Y;

    // Update the estimate's covariance matrix, reflecting the reduced uncertainty
    state->P_hat = (Eigen::Matrix<float, 6, 6>::Identity() - K * H) * state->P_tilde;
    

}

void kalman_update_gyro(Kalman_Rocket_State * state, float wx, float wy, float wz, Eigen::Matrix<float, 3, 3> R_gyro) {

    Eigen::Matrix<float, 3, 9> H;  // Measurement matrix for angular velocities
    H.setZero();
    H(0, 6) = 1;  // wx
    H(1, 7) = 1;  // wy
    H(2, 8) = 1;  // wz

    Eigen::Matrix<float, 3, 1> Z;  // Measurement matrix from the gyroscope
    Z << wx, wy, wz;

    Eigen::Matrix<float, 3, 1> Y = Z - H * state->X_tilde;  // Measurement residual

    Eigen::Matrix<float, 3, 3> S = H * state->P_tilde * H.transpose() + R_gyro;  // Residual covariance

    Eigen::Matrix<float, 9, 3> K = state->P_tilde * H.transpose() * S.inverse();  // Kalman Gain

    state->X_hat = state->X_hat + K * Y;  // Update state estimate

    state->P_hat = (Eigen::Matrix<float, 9, 9>::Identity() - K * H) * state->P_tilde;  // Update error covariance matrix

}

/*

struct SensRaw {
    adxl375_data        adxl; // accelerometer moins precis mais on peut lire de plus grosses accelerations
    adxl375_data        adxl_aux;

    bmi08_sensor_data_f bmi_accel;
    bmi08_sensor_data_f bmi_aux_accel;

    bmi08_sensor_data_f bmi_gyro; // Correspondance x,y,z / roll,pitch,yaw à déterminer
    bmi08_sensor_data_f bmi_aux_gyro;

    bmp3_data           bmp; // temperature and pressure
    bmp3_data           bmp_aux;

    SensRaw();
};

*/

void kalman_handle_data(Kalman_Rocket_State * state, Data data, AvState av_state) {

    if (state == NULL) {
        std::cerr << "Error: NULL Kalman_Rocket_State in kalman_handle_data" << "\n" << std::endl;
        return;
    }

    /* INITIALLY
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

   /* MY LINEAR VERSION FOR KALMAN_TEST.CPP WITH AV_DATA
   // Calculate the time elapsed since the last update in seconds
    float dt = (data.time - state->last_time) / 1000.0f; // Conversion from milliseconds to seconds
    //kalman_predict(state, dt);

    if (dt > 0 && (av_state.getCurrentState() == State::ASCENT || av_state.getCurrentState() == State::DESCENT)) {
        // If time has passed and the rocket is ignited, predict the next state
        kalman_predict(state, dt);
        //state->last_time = data.time; // Update the last processed time
        
        // If accelerometer data is available, update acceleration
        kalman_update_acc(state, data.acceleration);
        
        // If barometer data is available, convert it to altitude and update
        if (data.pressure > 0) {
            float altitude = pressure_to_altitude(data.pressure); // Convert pressure to altitude
            kalman_update_baro(state, altitude); // Update Kalman Filter with the altitude
        }
    }
    state->last_time = data.time;
    */

    /* MY LINEAR VERSION FOR NEW_KALMAN_TEST.CPP WITH AVDATA
    float dt = (data.time - state->last_time);

    if (dt > 0 && (av_state.getCurrentState() == State::ASCENT || av_state.getCurrentState() == State::DESCENT)) {
        kalman_predict(state, dt);
        kalman_update_acc(state, data.acceleration);
        
        if (data.pressure > 0) {
            //float altitude = pressure_to_altitude(data.pressure); // Convert pressure to altitude
            kalman_update_baro(state, data.pressure); // Update Kalman Filter with the altitude
        }
    }
    state->last_time = data.time;
    */

   /* MY LINEAR VERSION WITH SENSORS.H */

    SensRaw sens_raw = data.get_sensors().get_raw_data();
    float time = data.get_time();
    float dt = (time - state->last_time);

    if (dt <= 0) return;

    //TODO: Introduce sensor status verification!!!
    if (av_state.getCurrentState() == State::ASCENT || av_state.getCurrentState() == State::DESCENT) {

        kalman_predict(state, dt);

        // When multiple sensors: https://stackoverflow.com/questions/55813719/multi-sensors-fusion-using-kalman-filter
        //TODO: Determine in what order I should update the filter
        
        // Update accelerometer
        kalman_update_acc(state, sens_raw.adxl.z, state->R_acc_adxl);
        kalman_update_acc(state, sens_raw.adxl_aux.z, state->R_acc_adxl);
        kalman_update_acc(state, sens_raw.bmi_accel.z, state->R_acc_bmi);
        kalman_update_acc(state, sens_raw.bmi_aux_accel.z, state->R_acc_bmi);
        
        // Update barometer
        kalman_update_baro(state, sens_raw.bmp.pressure, sens_raw.bmp.temperature, state->R_baro_bmp);
        kalman_update_baro(state, sens_raw.bmp_aux.pressure, sens_raw.bmp_aux.temperature, state->R_baro_bmp);

        // Update gyroscope
        kalman_update_gyro(state, sens_raw.bmi_gyro.x, sens_raw.bmi_gyro.y, sens_raw.bmi_gyro.z, state->R_gyro_bmi);
        kalman_update_gyro(state, sens_raw.bmi_aux_gyro.x, sens_raw.bmi_aux_gyro.y, sens_raw.bmi_aux_gyro.z, state->R_gyro_bmi);

    }
    state->last_time = data.time;
}

Kalman_Rocket_State* kalman_entry() {

    // TO BE MODIFIED
    /* WITH KALMAN_TEST 
    static float p0 = 100500.710127303867f;
    static float alt0 = 165.0f;
    */

   /* WITH NEW_KALMAN_TEST */
    static float p0 = 99498.05f;
    static float alt0 = 0.0f;
    
    static float wx0 = 0.0f;
    static float wy0 = 0.0f;
    static float wz0 = 0.0f;

    Kalman_Rocket_State * state = nullptr;

    // in C++, the new operator throws a std::bad_alloc exception if it fails to allocate memory, 
    // rather than returning NULL.
    try {
        state = new Kalman_Rocket_State;
        
    } catch (const std::bad_alloc& e) {
        std::cerr << "Error: Couldn't create Kalman_Rocket_State in kalman_entry() due to memory allocation failure" << "\n" << std::endl;
        return nullptr;
    }

    kalman_setup(state, alt0, p0, wx0, wy0,  wz0);

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

std::tuple<float, float> KalmanFilter::UpdateAndGetAltitudeAndVelocity(State state, Data data, AvState av_state) {

    kalman_handle_data(state, data, av_state);
    return std::make_tuple(state->X_hat(0,0), state->X_hat(1,0)); // returns the estimated altitude and velocity
}
