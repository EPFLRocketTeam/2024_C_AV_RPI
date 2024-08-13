#include <stdio.h>
#include <math.h>
#include <iostream>
#include <tuple>

// Put Eigen (extracted zip file) in a directory where you put your existing working header file
#include "flightControl/eigen-3.4.0/Eigen/Dense"
#include "flightControl/eigen-3.4.0/unsupported/Eigen/MatrixFunctions"
#include "AvState.h"
#include "Kalman.h"
#include "data/sensors.h"
#include "data/data.h"

/* A Faire
- when averaging sensors, introduce weights according to their accuracy
*/

// State : z, vx, vy, vz, ax, ay, az, wx, wy, wz
// (altitude, velocity on x, velocity on y, velocity on z, acceleration on x, acceleration on y, acceleration on z,
// angular velocity components)

// Constants
#define T       288.15 // reference temperature (K)

// Variances
// TODO: Determine variances for sensors and initial uncertainties

// For R
#define sigma_z_baro_bmp    0.0053 // for Pa
#define sigma_z_acc_adxl   8.47819771902424e-07 // for m
#define sigma_z_acc_bmi    8.47819771902424e-07 // for m // a faire selon x, y, z
#define sigma_z_gyro_x_bmi  1.42999893049913e-07
#define sigma_z_gyro_y_bmi  2.08838117409406e-07
#define sigma_z_gyro_z_bmi  1.18100489973845e-07

// Used to initialize Q (je crois que Q c'est a quel point on a confiance en prediction (plus c'est bas, plus c'est confiant))
// Helps KF to appropriately incorporate new acceleration data while balancing it 
// with the existing state estimate and its uncertainty
// Q describes the statistical properties (specifically, the covariance) of w (statistical properties (specifically, the covariance) of w)
/*
In many practical applications, Q is determined based on empirical data or experimentation. You might start with a guess 
and refine Q based on how well the filter performs in predicting the system state.
Consider the physics or the dynamics of your system. If certain states are expected to experience more noise or uncertainty 
(e.g., due to mechanical vibrations, thermal fluctuations), Q should reflect this.
In some advanced scenarios, Q can be adapted over time based on residuals (the differences between predicted and measured states), 
which can help the filter adjust to changing system dynamics.
*/
#define sigma_ax    5.87073608617591e-05 // for m/s²
#define sigma_ay    9.03770197486529e-05 // for m/s²
#define sigma_az    0.000701256732495510 // for m/s²
#define sigma_wx    2.13285457809695e-07 // for rad/s
#define sigma_wy    7.42908438061041e-07 // for rad/s
#define sigma_wz    2.14470377019749e-06 // for rad/s

// - set to low values if altitude and velocity are primarily driven by the measurements and 
//   system model rather than by unmodeled process noise
// - to adjust based on simulations
#define sigma_z  1e-9
#define sigma_vx 1e-9
#define sigma_vy 1e-9
#define sigma_vz 1e-9

// Consider modeling the process noise for angular velocities more dynamically, potentially 
// updating your noise covariance matrix Q based on observed system behavior or using adaptive 
// filtering techniques to better cope with non-linear dynamics and non-stationary noise.

// Accelerometer and gyroscope calibration (see https://rocket-team.epfl.ch/icarus/gnc/navigation)


// alt0: reference altitude for the barometer 
// p0: pressure at alt0
void kalman_setup(Kalman_Rocket_State * state, double alt0, double wx0, double wy0, double wz0) {

    std::cout << "In kalman_setup" << "\n" << std::endl;
    std::cout << "\n" << std::endl;

    // State vector
    state->X_tilde << alt0, 0, 0, 0, 0, 0, 0, wx0, wy0, wz0;
    state->X_hat << state->X_tilde;

    // Covariance matrix (uncertainty associated with the state)
    state->P_tilde.diagonal() << 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9, 1e-9;
    state->P_hat << state->P_tilde;  

    // baro_noise in icarus
    state->R_baro_bmp << powf(sigma_z_baro_bmp, 2.0);

    state->R_acc_adxl.diagonal()<< powf(sigma_z_acc_adxl, 2.0), powf(sigma_z_acc_adxl, 2.0), powf(sigma_z_acc_adxl, 2.0);
    state->R_acc_bmi.diagonal() << powf(sigma_z_acc_bmi, 2.0), powf(sigma_z_acc_bmi, 2.0), powf(sigma_z_acc_bmi, 2.0);
    
    // Determine if noises are independent (if not, R_gyro is not diagonal)
    state->R_gyro_bmi.diagonal() << powf(sigma_z_gyro_x_bmi, 2.0), powf(sigma_z_gyro_y_bmi, 2.0), powf(sigma_z_gyro_z_bmi, 2.0);

   // Covariance noise matrix (noise of the predict step) (accel_bias and gyro_bias in icarus)
   state->Q.diagonal() <<  
                            powf(sigma_z, 2.0f), 
                            powf(sigma_vx, 2.0f), powf(sigma_vy, 2.0f), powf(sigma_vz, 2.0f),
                            powf(sigma_ax, 2.0f), powf(sigma_ay, 2.0f), powf(sigma_az, 2.0f),
                            powf(sigma_wx, 2.0f), powf(sigma_wy, 2.0f), powf(sigma_wz, 2.0f);

    // will be overwritten?
    state->F.setIdentity(10, 10);

    // Because control input w is [ax, ay, az, wx, wy, wz]
    state->G << 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, // acceleration noise directly affects acceleration
                0, 1, 0, 0, 0, 0,
                0, 0, 1, 0, 0, 0,
                0, 0, 0, 1, 0, 0, // gyro noise affects angular velocities
                0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 1;
    
    //std::cout << "G done" << "\n" << std::endl;

    state->last_time = 0;
    state->last_temp_bmi = T; // To initialize correctly
    state->last_temp_bmi_aux = T;
}


void kalman_predict(Kalman_Rocket_State * state, Eigen::Matrix<double, 6, 1> u, /*Eigen::Matrix<double, 10, 10> w,*/ double dt) {

    std::cout << "In kalman_predict" << "\n" << std::endl;

   /* MY LINEAR VERSION */

    // Extract control inputs
    double u_ax = u(0), u_ay = u(1), u_az = u(2);
    double u_wx = u(3), u_wy = u(4), u_wz = u(5);

    // Define the state transition matrix F for the given system
    // Update F to include the effect of acceleration on velocity and altitude
    // state->F << 1, 0, 0, dt, 0, 0, 0.5 * dt * dt, 0, 0, 0,                            // altitude (z)
    //             0, 1, 0, 0, dt, 0, 0, 0, 0, 0,                                       // velocity (vx)
    //             0, 0, 1, 0, 0, dt, 0, 0, 0, 0,                                    // velocity (vy)
    //             0, 0, 0, 1, 0, 0, dt, 0, 0, 0,                                       // velocity (vz)
    //             0, 0, 0, 0, 1, 0, 0, 0, 0, 0,                                       // acceleration (ax)
    //             0, 0, 0, 0, 0, 1, 0, 0, 0, 0,                                       // acceleration (ay)
    //             0, 0, 0, 0, 0, 0, 1, 0, 0, 0,                                      // acceleration (az)
    //             0, 0, 0, 0, 0, 0, 0, 1, 0, 0,                                       // angular velocity (wx)
    //             0, 0, 0, 0, 0, 0, 0, 0, 1, 0,                                       // angular velocity (wy)
    //             0, 0, 0, 0, 0, 0, 0, 0, 0, 1;                                       // angular velocity (wz)


    // Update the state transition matrix F appropriately for the system dynamics
    state->F.setIdentity(10, 10);

    // Update matrix F for direct relationships
    state->F(0, 3) = dt;               // z is updated by vz
    state->F(1, 4) = dt;               // vx is updated by ax
    state->F(2, 5) = dt;               // vy is updated by ay
    state->F(3, 6) = dt;               // vz is updated by az


    // First order integration of velocity to update position
    state->X_tilde(0) += state->X_tilde(3) * dt + 0.5 * u_az * dt * dt; // Update altitude z with current vz and acceleration az
    state->X_tilde(1) += u_ax * dt; // Update vx with acceleration ax
    state->X_tilde(2) += u_ay * dt; // Update vy with acceleration ay
    state->X_tilde(3) += u_az * dt; // Update vz with acceleration az

    // Update angular velocities
    state->X_tilde(7) += u_wx * dt; // Update angular velocity wx
    state->X_tilde(8) += u_wy * dt; // Update angular velocity wy
    state->X_tilde(9) += u_wz * dt; // Update angular velocity wz


    // double wx = state->X_tilde(7);
    // double wy = state->X_tilde(8);
    // double wz = state->X_tilde(9);

    //std::cout << "wx: " << wx << "\n" << std::endl;
    ////std::cout << "wy: " << wy << "\n" << std::endl;
    //std::cout << "wz: " << wz << "\n" << std::endl;

    // Simple rotation effect based on angular velocities
    Eigen::Matrix<double, 3, 3> rotation_effect;
    rotation_effect << 0, -u_wz * dt, u_wy * dt,
                       u_wz * dt, 0, -u_wx * dt,
                       -u_wy * dt, u_wx * dt, 0;

    // std::cout << "rotation_effect: " << "\n" << std::endl;
    // std::cout << rotation_effect << "\n" << std::endl;

    // Apply the rotation effect to the velocity components in the state vector
    Eigen::Matrix<double, 3, 1> velocity = state->X_tilde.segment<3>(1) + rotation_effect * state->X_tilde.segment<3>(1);
    state->X_tilde.segment<3>(1) = velocity;

    // Predict the next state vector
    state->X_tilde = state->F * state->X_hat + state->G * u;

    // std::cout << "X_tilde after:" << "\n" << std::endl;
    // std::cout << state->X_hat << "\n" << std::endl;

    // Eigen::Matrix<double, 10, 10> Q = state->G * w * state->G.transpose() + state->Q;

    state->P_tilde = state->F * state->P_hat * state->F.transpose() + state->Q;
    
}

// Converts barometric pressure readings to altitude using the International Standard Atmosphere model
// pressure in Pascals ; temperature in Kelvins
// Returns The altitude above sea level in meters
double pressure_to_altitude(double pressure, double temperature) {

    // Constants for the International Standard Atmosphere
    static const double P0 = 101325.0;  // Sea level standard atmospheric pressure (in Pascals)
    static const double T0 = 288.15;    // Sea level standard temperature (in Kelvins)
    static const double L = 0.0065;     // Temperature lapse rate (in K/m)
    static const double g = 9.80665;    // standard gravity (m/s²)
    static const double M = 0.0289644; // air's molar mass (kg/mol)
    static const double R = 8.3144598; // ideal gas constant (J/(mol*K))

    return (1.0 - powf(pressure / P0, R * L / (g * M))) * temperature / L;
}

void kalman_update_baro(Kalman_Rocket_State * state, double p, double temperature, Eigen::Matrix<double, 1, 1> R_baro) {

    std::cout << "In kalman_update_baro" << "\n" << std::endl;

   Eigen::Matrix<double, 1, 10> H; // Measurement matrix for altitude
    H.setZero();
    H(0, 0) = 1;  // Altitude is directly measured
    //std::cout << "H done" << "\n" << std::endl;

    Eigen::Matrix<double, 1, 1> Z;
    Z << pressure_to_altitude(p, temperature);
    //std::cout << "Z done" << "\n" << std::endl;

    Eigen::Matrix<double, 1, 1> Y;
    Y << Z - H * state->X_tilde;  // Z is a 1x1 matrix, H * state->X_tilde results in a 1x1 matrix
    //std::cout << "Y done" << "\n" << std::endl;

    Eigen::Matrix<double, 1, 1> S = H * state->P_tilde * H.transpose() + R_baro;  // Residual covariance
    //std::cout << "S done" << "\n" << std::endl;

    Eigen::Matrix<double, 10, 1> K = state->P_tilde * H.transpose() * S.inverse();  // Kalman Gain
    //std::cout << "K done" << "\n" << std::endl;

    state->X_hat = state->X_tilde + K * Y;  // Update state estimate
    //std::cout << "X_hat done" << "\n" << std::endl;

    state->P_hat = (Eigen::Matrix<double, 10, 10>::Identity() - K * H) * state->P_tilde;  // Update error covariance matrix
    //std::cout << "P_hat done" << "\n" << std::endl;
}

void kalman_update_acc(Kalman_Rocket_State * state, double ax, double ay, double az, Eigen::Matrix<double, 3, 3> R_acc) {

    std::cout << "In kalman_update_acc" << "\n" << std::endl;
    std::cout << "ax: " << ax << "\n" << std::endl;
    std::cout << "ay: " << ay << "\n" << std::endl;
    std::cout << "az: " << az << "\n" << std::endl;

   /* LINEAR VERSION AND EXTENDED VERSION */
   // H matrix maps the state vector's acceleration component to the measured acceleration
    Eigen::Matrix<double, 3, 10> H;
    H.setZero();
    H(0, 4) = 1;  // ax directly affects the state
    H(1, 5) = 1;  // ay directly affects the state
    H(2, 6) = 1;  // az directly affects the state
    // std::cout << "H" << "\n" << std::endl;
    // std::cout << H << "\n" << std::endl;

    // Y is the measurement residual: the difference between the measured acceleration
    // and the predicted acceleration from the state vector
    Eigen::Matrix<double, 3, 1> Y;
    Y << ax - state->X_tilde(4),  // Residual for ax
         ay - state->X_tilde(5),  // Residual for ay
         az - state->X_tilde(6);  // Residual for az
    std::cout << "Y" << "\n" << std::endl;
    std::cout << Y << "\n" << std::endl;

    // S is the residual covariance, incorporating both the measurement noise and the
    // uncertainty of the predicted state as it relates to the measurement
    Eigen::Matrix<double, 3, 3> S = H * state->P_tilde * H.transpose() + R_acc;
    // std::cout << "S" << "\n" << std::endl;
    // std::cout << S << "\n" << std::endl;

    // K is the Kalman Gain, determining how much the measurement should influence the state update
    Eigen::Matrix<double, 10, 3> K = state->P_tilde * H.transpose() * S.inverse();
    std::cout << "K" << "\n" << std::endl;
    std::cout << K << "\n" << std::endl;

    //std::cout << "X_hat before:" << "\n" << std::endl;
    //std::cout << state->X_hat << "\n" << std::endl;

    // Update the state estimate using the measurement residual, scaled by the Kalman gain
    state->X_hat = state->X_tilde + K * Y;
    std::cout << "X_tilde" << "\n" << std::endl;
    std::cout << state->X_tilde << "\n" << std::endl;
    std::cout << "K*Y" << "\n" << std::endl;
    std::cout << K*Y << "\n" << std::endl;


    // std::cout << "X_hat after:" << "\n" << std::endl;
    // std::cout << state->X_hat << "\n" << std::endl;

    // Update the estimate's covariance matrix, reflecting the reduced uncertainty
    state->P_hat = (Eigen::Matrix<double, 10, 10>::Identity() - K * H) * state->P_tilde;
    //std::cout << "P_hat done" << "\n" << std::endl;

    std::cout << "State Estimated" << "\n" << std::endl;
    std::cout << state->X_hat << "\n" << std::endl;
}

void kalman_update_gyro(Kalman_Rocket_State * state, double wx, double wy, double wz, Eigen::Matrix<double, 3, 3> R_gyro) {

    std::cout << "In kalman_update_gyro" << "\n" << std::endl;

    Eigen::Matrix<double, 3, 10> H;  // Measurement matrix for angular velocities
    H.setZero();
    H(0, 7) = 1;  // wx
    H(1, 8) = 1;  // wy
    H(2, 9) = 1;  // wz
    //std::cout << "H done" << "\n" << std::endl;

    Eigen::Matrix<double, 3, 1> Z;  // Measurement matrix from the gyroscope
    Z << wx, wy, wz;
    //std::cout << "Z done" << "\n" << std::endl;

    Eigen::Matrix<double, 3, 1> Y = Z - H * state->X_tilde;  // Measurement residual
    //std::cout << "Y done" << "\n" << std::endl;

    Eigen::Matrix<double, 3, 3> S = H * state->P_tilde * H.transpose() + R_gyro;  // Residual covariance
    //std::cout << "S done" << "\n" << std::endl;

    Eigen::Matrix<double, 10, 3> K = state->P_tilde * H.transpose() * S.inverse();  // Kalman Gain
    //std::cout << "K done" << "\n" << std::endl;

    state->X_hat = state->X_hat + K * Y;  // Update state estimate
    //std::cout << "X_hat done" << "\n" << std::endl;

    state->P_hat = (Eigen::Matrix<double, 10, 10>::Identity() - K * H) * state->P_tilde;  // Update error covariance matrix
    //std::cout << "P_hat done" << "\n" << std::endl;

    std::cout << "State Estimated" << "\n" << std::endl;
    std::cout << state->X_tilde << "\n" << std::endl;
}

Eigen::Matrix<double, 6, 1> fuse_sensors(SensRaw* sens_raw_ptr, SensStatus* sens_status_ptr) {

    SensRaw sens_raw = *sens_raw_ptr;
    SensStatus status = *sens_status_ptr;

    // A reflechir: 
    // - if no valid data then previous data or 0?
    // - for acceleration, importance/more weight for one type of sensor?

    Eigen::Matrix<double, 6, 1> u;
    u.setZero(6,1);

    int nbr_accel_sensors = 0;
    if (status.adxl_status & ADXL375_DATA_READY) { //in include/sensor/adxl375.h
        u(0) += sens_raw.adxl.x;
        u(1) += sens_raw.adxl.y;
        u(2) += sens_raw.adxl.z;
        ++nbr_accel_sensors;
    }
    if (status.adxl_aux_status & ADXL375_DATA_READY) {
        u(0) += sens_raw.adxl_aux.x;
        u(1) += sens_raw.adxl_aux.y;
        u(2) += sens_raw.adxl_aux.z;
        ++nbr_accel_sensors;
    }
    if (status.bmi_accel_status & BMI08_ACCEL_DATA_READY_INT) { // in include/sensor/bmi08_defs.h
        u(0) += sens_raw.bmi_accel.x;
        u(1) += sens_raw.bmi_accel.y;
        u(2) += sens_raw.bmi_accel.z;
        ++nbr_accel_sensors;
    }
    if (status.bmi_aux_accel_status & BMI08_ACCEL_DATA_READY_INT) {
        u(0) += sens_raw.bmi_aux_accel.x;
        u(1) += sens_raw.bmi_aux_accel.y;
        u(2) += sens_raw.bmi_aux_accel.z;
        ++nbr_accel_sensors;
    }
    if (nbr_accel_sensors > 0) {
        u(0) /= nbr_accel_sensors;
        u(1) /= nbr_accel_sensors;
        u(2) /= nbr_accel_sensors;
    }

    // Update gyroscope
    // S'assurer que c'est en rad/s
    int nbr_gyro_sensors = 0;
    if (status.bmi_gyro_status & BMI08_GYRO_DATA_READY_INT) {
        u(3) += sens_raw.bmi_gyro.x;
        u(4) += sens_raw.bmi_gyro.y;
        u(5) += sens_raw.bmi_gyro.z;
        ++nbr_gyro_sensors;
    }
    if (status.bmi_aux_gyro_status & BMI08_GYRO_DATA_READY_INT) {
        u(3) += sens_raw.bmi_aux_gyro.x;
        u(4) += sens_raw.bmi_aux_gyro.y;
        u(5) += sens_raw.bmi_aux_gyro.z;
        ++nbr_gyro_sensors;
    }

    if (nbr_gyro_sensors > 0) {
        u(3) /= nbr_gyro_sensors;
        u(4) /= nbr_gyro_sensors;
        u(5) /= nbr_gyro_sensors;
    }
    

    return u;
}

void kalman_handle_data(Kalman_Rocket_State * state, Data data, AvState av_state) {

    if (state == NULL) {
        std::cerr << "Error: NULL Kalman_Rocket_State in kalman_handle_data" << "\n" << std::endl;
        return;
    }

    SensRaw sens_raw = data.get_sensors().get_raw_data();
    SensStatus status = data.get_sensors().get_status();
    double time = data.get_time();
    double dt = (time - state->last_time);

    if (dt <= 0) return;

    // TODO: determine quoi faire de ca
    Eigen::Matrix<double, 6, 1> u;
    Eigen::Matrix<double, 10, 10> w;

    if (av_state.getCurrentState() == State::ASCENT || av_state.getCurrentState() == State::DESCENT) {

        Eigen::Matrix<double, 6, 1> u = fuse_sensors(&sens_raw, &status);
        kalman_predict(state, u, dt);

        // When multiple sensors: https://stackoverflow.com/questions/55813719/multi-sensors-fusion-using-kalman-filter
        //TODO: Determine in what order I should update the filter
        
        //TODO: Verifiy correctness checking for sensors

       

        // Update barometer
        if ((status.bmp_status.sensor.cmd_rdy & BMP3_CMD_RDY) &&
            (status.bmp_status.sensor.drdy_press & BMP3_DRDY_PRESS)) { // in include/sensor/bmp3_defs.h

            if (status.bmp_status.sensor.drdy_temp & BMP3_DRDY_TEMP) {
                //std::cout << "bmp_status.sensor.drdy_temp" << "\n" << std::endl;
                state->last_temp_bmi = sens_raw.bmp.temperature;
            }
            //std::cout << "bmp_status.sensor.drdy_press" << "\n" << std::endl;
            kalman_update_baro(state, sens_raw.bmp.pressure, state->last_temp_bmi, state->R_baro_bmp);
        }
        
        if ((status.bmp_aux_status.sensor.cmd_rdy & BMP3_CMD_RDY) &&
            (status.bmp_aux_status.sensor.drdy_press & BMP3_DRDY_PRESS)) {

            if (status.bmp_aux_status.sensor.drdy_temp & BMP3_DRDY_TEMP) {
                //std::cout << "bmp_aux_status.sensor.drdy_temp" << "\n" << std::endl;
                state->last_temp_bmi_aux = sens_raw.bmp_aux.temperature;
            }
            //std::cout << "bmp_aux_status.sensor.drdy_press" << "\n" << std::endl;
            kalman_update_baro(state, sens_raw.bmp_aux.pressure, state->last_temp_bmi_aux, state->R_baro_bmp);
        }


        
        
        

    }
    state->last_time = data.get_time();
}

Kalman_Rocket_State* kalman_entry() {

    // TO BE MODIFIED
    /* WITH KALMAN_TEST
    static double p0 = 100500.710127303867f;
    static double alt0 = 165.0f;
    */

   /* WITH NEW_KALMAN_TEST*/
    static double p0 = 99498.05;
    static double alt0 = 0.0;

    static double wx0 = 0.0;
    static double wy0 = 0.0;
    static double wz0 = 0.0;


    Kalman_Rocket_State * state = nullptr;

    // in C++, the new operator throws a std::bad_alloc exception if it fails to allocate memory, 
    // rather than returning NULL.
    try {
        state = new Kalman_Rocket_State;
        
    } catch (const std::bad_alloc& e) {
        std::cerr << "Error: Couldn't create Kalman_Rocket_State in kalman_entry() due to memory allocation failure" << "\n" << std::endl;
        return nullptr;
    }

    kalman_setup(state, alt0, wx0, wy0, wz0);

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

//TODO: Modify returned velocity
std::tuple<double, Vector3> KalmanFilter::UpdateAndGetAltitudeAndVelocity(Data data, AvState av_state) {

    kalman_handle_data(state, data, av_state);
    Vector3 velocityVector = {state->X_hat(1,0), state->X_hat(2,0), state->X_hat(3,0)};
    return std::make_tuple(state->X_hat(0,0), velocityVector); // returns the estimated altitude and velocity
}

Vector3 KalmanFilter::GetAcceleration() {
    return {state->X_hat(4,0), state->X_hat(5,0), state->X_hat(6,0)};
}