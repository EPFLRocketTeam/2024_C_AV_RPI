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
// altitude, velocity, acceleration, initial pressure, , initial altitude


// Constants

#define g 		9.81f // standard gravity
#define M 		0.02897f // air's molar mass
#define R		8.3145f // ideal gas constant
#define T		298.15f // reference temperature

// Variances

#define sigma_z_baro 	5.0f
#define sigma_z_acc 	25.0f

#define sigma_a		1.0f
#define sigma_p0	0.1f
#define sigma_k		1e-9f


// alt0 : reference altitude for the barometer and its pressure
void kalman_setup(Kalman_Rocket_State * state, float alt0, float p0) {

    state->X_tilde << alt0, 0.0f, 0.0f, p0, M/(R*T), alt0;

    state->X_hat << state->X_tilde;
    
   state->P_tilde.diagonal() << 25.0, 0.25, 0.25, 25.0, 1e-12, 25.0;

    state->P_hat << state->P_tilde;

    state->R_baro << powf(sigma_z_baro, 2.0f);

    state->R_acc << powf(sigma_z_acc, 2.0f);

    state->Q.diagonal() << powf(sigma_a, 2.0f),  powf(sigma_p0, 2.0f), powf(sigma_k, 2.0f);

    state->F <<     0,  1,  0,  0,  0,  0,
                    0,  0,  1,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0;

    
    state->G << 0,  0,  0,
                0,  0,  0,
                1,  0,  0,
                0,  0,  0,
                0,  0,  0,
                0,  0,  0
    ;

    state->last_time = 0;
}


void kalman_predict(Kalman_Rocket_State * state, float dt) {

    /* LEUR VERSION */
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
}


void kalman_update_baro(Kalman_Rocket_State * state, float p) {

    float h = state->X_tilde(0, 0);
	float p0 = state->X_tilde(3, 0);
	float k = state->X_tilde(4, 0);
	float h0 = state->X_tilde(5, 0);

    // Expected pressure at the current estimated altitude (barometric formula)
	float p_est = p0 * expf(k * g * (h0-h));

    // Jacobian
    Eigen::Matrix<float, 1, 6> H;
	H << -k*g*p_est, 0.0, 0.0, p_est/p0, g*(h0-h)*p_est, k*g*p_est;
    // possibility: -k*g*p0*expf(-k*g*(h-h0)), 0.0, 0.0, expf(-k*g*(h-h0)), -g*(h-h0)*p0*expf(-k*g*(h-h0)), k*g*p0*expf(-k*g*(h-h0));

    // Kalman Gain
	Eigen::Matrix<float, 6, 1> K;
	K << state->P_tilde*H.transpose()*(H*state->P_tilde*H.transpose() + state->R_baro).inverse();

    // Update state estimate
	state->X_hat << state->X_tilde + K*(p - p_est);

    // Update error covariance matrix
	state->P_hat << (Eigen::MatrixXf::Identity(6,6) - K*H)*state->P_tilde;

}


void kalman_update_acc(Kalman_Rocket_State * state, float a) {

    Eigen::Matrix<float, 1, 6> H;
    H << 0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
    
    Eigen::Matrix<float, 6, 1> K;
    K << state-> P_tilde * H.transpose() * (H * state->P_tilde * H.transpose() + state->R_acc).inverse();

    state->X_hat << state->X_tilde + K * (a - state->X_tilde(2));

    state->P_hat << (Eigen::MatrixXf::Identity(6,6) - K * H) * state->P_tilde;


}


void kalman_handle_data(Kalman_Rocket_State * state, AvData data) {


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

}


Kalman_Rocket_State* kalman_entry() {

    // TO BE MODIFIED
    static float p0 = 100500.710127303867f;
	static float alt0 = 165.0f;

    Kalman_Rocket_State * state = new Kalman_Rocket_State;
    
    // To handle
    if (state == NULL) {
        return NULL;
    }

    kalman_setup(state, alt0, p0);

    return state;
}


// Defining KalmanFilter

KalmanFilter::KalmanFilter() {
    state = kalman_entry();

    if (state == NULL) {
        // ======= HANDLE THE CASE HERE =======
    }
}

std::tuple<float, float> KalmanFilter::UpdateAndGetAltitudeAndVelocity(AvData data) {

    kalman_handle_data(state, data);
    return std::make_tuple(state->X_hat(0,0), state->X_hat(1,0)); // returns the estimated altitude and velocity
}