#include "kalman.h"
#include "sensors.h"
#include "data.h"
#include <Eigen/Dense>
#include "kalman_params.h"

namespace {
    auto start = std::chrono::high_resolution_clock::now();

    uint32_t millis() {
        // Calculate the elapsed time, converting duration to milliseconds
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now-start);
        return static_cast<uint32_t>(duration.count());
    };
}


Kalman::Kalman( float estimate_covariance_gyro, 
                float estimate_covariance_accel,
                float estimate_covariance_orientation,
                float gyro_cov, 
                float gyro_bias_cov,
                float accel_proc_cov, 
                float accel_bias_cov,
                float gps_obs_cov, 
                float alt_obs_cov)
    : orientation_estimate(Quaternion(1,0,0,0)),
      velocity_estimate(Eigen::Vector3f::Zero()),
      position_estimate(Eigen::Vector3f::Zero()),
      gyro_bias(Eigen::Vector3f::Zero()),
      accelerometer_bias(Eigen::Vector3f::Zero()),
      state(Eigen::VectorXf::Zero(15)),
      observation_covariance(Eigen::Matrix3f::Zero()),
      gyro_cov_mat(Eigen::Matrix3f::Identity() * gyro_cov),
      gyro_bias_cov_mat(Eigen::Matrix3f::Identity() * gyro_bias_cov),
      accel_cov_mat(Eigen::Matrix3f::Identity() * accel_proc_cov),
      accel_bias_cov_mat(Eigen::Matrix3f::Identity() * accel_bias_cov),
      G(Eigen::MatrixXf::Zero(15, 15)),
      initial_azimuth(INITIAL_AZIMUTH),
      g(GRAVITY) {
    

    // Initialize covariance matrices
    initial_estimate_covariance.setZero();
    initial_estimate_covariance(0, 0) = estimate_covariance_orientation;
    initial_estimate_covariance(1, 1) = estimate_covariance_orientation;
    initial_estimate_covariance(2, 2) = estimate_covariance_orientation;
    initial_estimate_covariance(9, 9) = estimate_covariance_gyro;
    initial_estimate_covariance(10, 10) = estimate_covariance_gyro;
    initial_estimate_covariance(11, 11) = estimate_covariance_gyro;
    initial_estimate_covariance(12, 12) = estimate_covariance_accel;
    initial_estimate_covariance(13, 13) = estimate_covariance_accel;
    initial_estimate_covariance(14, 14) = estimate_covariance_accel;

    estimate_covariance = initial_estimate_covariance;

    
    // Initialize observation covariance
    observation_covariance(0, 0) = gps_obs_cov;
    observation_covariance(1, 1) = gps_obs_cov;
    observation_covariance(2, 2) = alt_obs_cov;
    
    // Initialize constant parts of G matrix
    for (int i = 0; i < 3; ++i) {
        G(i, i + 9) = -1.0;
        G(i + 6, i + 3) = 1.0;
    }
}

void Kalman::check_static(Data::GoatReg reg, const DataDump& dump) {
    // updates is_static 
    if(dump.av_state == State::READY){
        is_static = false; // Stop the periodic calibration
    }
}

 

Eigen::MatrixXf Kalman::process_covariance() {
    Eigen::MatrixXf Q = Eigen::MatrixXf::Zero(15, 15);
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Q(i, j) = gyro_cov_mat(i, j) * time_delta + 
                      gyro_bias_cov_mat(i, j) * (time_delta * time_delta * time_delta) / 3.0;
            Q(i, j + 9) = -gyro_bias_cov_mat(i, j) * (time_delta * time_delta) / 2.0;
            Q(i + 3, j + 3) = accel_cov_mat(i, j) * time_delta + 
                              accel_bias_cov_mat(i, j) * (time_delta * time_delta * time_delta) / 3.0;
            Q(i + 3, j + 12) = -accel_bias_cov_mat(i, j) * (time_delta * time_delta) / 2.0;
            Q(i + 9, j) = -gyro_bias_cov_mat(i, j) * (time_delta * time_delta) / 2.0;
            Q(i + 9, j + 9) = gyro_bias_cov_mat(i, j) * time_delta;
            Q(i + 12, j + 3) = -accel_bias_cov_mat(i, j) * (time_delta * time_delta) / 2.0;
            Q(i + 12, j + 12) = accel_bias_cov_mat(i, j) * time_delta;
        }
    }
    return Q;
}

void Kalman::calculate_initial_orientation() {
    // UP vector in the body frame
    const Eigen::Vector3f U = accel_static_avg / g;
    
    // Temporary North vector in the body frame (arbitrary azimuth)
    const Eigen::Vector3f tN = U.cross(Eigen::Vector3f(1.0f, 0.0f, 0.0f)).normalized();
    
    // Temporary East vector in the body frame
    const Eigen::Vector3f tE = tN.cross(U).normalized();
    
    // Temporary rotation matrix
    const Eigen::Matrix3f tR = (Eigen::Matrix3f() << 
        tE(0), tE(1), tE(2),
        tN(0), tN(1), tN(2),
        U(0), U(1), U(2)).finished();

    const Quaternion tq0 = rot_matrix_to_quat(tR);
    // Arbitrary azimuth
    const float t_initial_azimuth = azimuth_of_quaternion(tq0);

    // Rotate the North and East vectors to the desired azimuth
    const float beta = initial_azimuth - t_initial_azimuth;
    const Eigen::Matrix3f rot_beta = (Eigen::Matrix3f() << 
        std::cos(beta), -std::sin(beta), 0.0f,
        std::sin(beta), std::cos(beta), 0.0f,
        0.0f, 0.0f, 1.0f).finished();

    const Eigen::Matrix3f R = rot_beta * tR;
    orientation_estimate = rot_matrix_to_quat(R);
}

void Kalman::fuse_IMUs(const NavSensors& nav_sensors, Eigen::Vector3f& output_gyro_meas, Eigen::Vector3f& output_acc_meas) {
    // TODO : Make it by . Meanwhile only use one IMU

    // Question : What are the units of the bmi IMU ? acc_meas and gyro_meas should be in SI units

    // Extract acceleration data from the primary BMI sensor
    output_acc_meas << nav_sensors.bmi_accel.x, nav_sensors.bmi_accel.y, nav_sensors.bmi_accel.z;

    // Extract gyroscope data from the primary BMI sensor
    output_gyro_meas << nav_sensors.bmi_gyro.x, nav_sensors.bmi_gyro.y, nav_sensors.bmi_gyro.z;
}

// Convert atmospheric pressure (hPa) to altitude (m) using the barometric formula
double pressure_to_altitude(double pressure_hpa) {
    const double P0 = 1013.25;  // Standard sea-level pressure in hPa
    const double T0 = 288.15;   // Standard temperature at sea level in Kelvin (15°C)
    const double L = 0.0065;    // Temperature lapse rate (K/m)
    const double g = 9.80665;   // Gravity (m/s²)
    const double R = 287.05;    // Specific gas constant for dry air (J/kg·K)

    return (T0 / L) * (1 - std::pow(pressure_hpa / P0, (R * L / g)));
}

void Kalman::gps_pressure_to_obs(const NavSensors& nav_sensors, const NavigationData& nav_data, Eigen::Vector3f& gps_meas, float& alt_meas) {
    const double delta_lat = nav_data.position.lat - initial_lat;
    const double delta_lon = nav_data.position.lng - initial_lon;
    const double lat_avg = (nav_data.position.lat + initial_lat)/ 2.0;

    gps_meas(0) = delta_lon * DEG_TO_RAD * EARTH_RADIUS * std::cos(lat_avg * DEG_TO_RAD);
    gps_meas(1) = delta_lat * DEG_TO_RAD * EARTH_RADIUS;

    // TODO? detect failure of one of the pressure sensors (maybe using gps alt to do a majority vote?)
    const float avg_pressure = (nav_sensors.bmp.pressure + nav_sensors.bmp_aux.pressure)/2.0f;
    alt_meas = pressure_to_altitude(avg_pressure) - initial_alt;
}

void Kalman::predict(const NavSensors& nav_sensors, const NavigationData& nav_data) {

    Eigen::Vector3f gyro_meas;
    Eigen::Vector3f acc_meas;
    fuse_IMUs(nav_sensors, gyro_meas, acc_meas);
    current_accel = {acc_meas(0), acc_meas(1), acc_meas(2)};

    time_delta = (millis() - last_measurement_time) / 1000.0f;
    last_measurement_time = millis();
    
    // TODO : I should probably put this in a different function
    if (is_static) {
        // If the rocket is static, and it hasn't calibrated in a while, do a new calibration
        if(millis() - last_static_calib_time > static_recalibration_interval) {
            is_calibrating = true;
            static_counter = 0;
            gyro_static_sum.setZero();
            accel_static_sum.setZero();
            static_calib_start_time = millis();
            last_static_calib_time = millis();
        }
        
        // If the rocket is static, and it is calibrating, add the measurements to the sum
        if(is_calibrating && millis() - static_calib_start_time < static_calib_duration) {
            gyro_static_sum += gyro_meas;
            accel_static_sum += acc_meas;
            static_counter++;
        }
        
        // If the rocket is static, and it has finished calibrating, calculate the gyro bias, the acceleration norm and the initial orientation
        if(is_calibrating && millis() - static_calib_start_time >= static_calib_duration) {
            gyro_bias = gyro_static_sum / static_counter;
            accel_static_avg = accel_static_sum / static_counter;
            is_calibrating = false;
            g = accel_static_avg.norm();
            calculate_initial_orientation();

            initial_lat = nav_data.position.lat;
            initial_lon = nav_data.position.lng;
            initial_alt = pressure_to_altitude((nav_sensors.bmp.pressure + nav_sensors.bmp_aux.pressure)/2);
        }
    }

    // Correct measurements
    const Eigen::Vector3f gyro_meas_corrected = gyro_meas - gyro_bias;
    const Eigen::Vector3f acc_meas_corrected = acc_meas - accelerometer_bias;

    // Compute world acceleration
    const Eigen::Matrix3f R = quat_to_matrix(orientation_estimate);
    
    Eigen::Vector3f acc_world = R * acc_meas_corrected;
    acc_world(2) -= g;

    // Compute quaternion derivative
    const Eigen::Vector3f q_vector(orientation_estimate.vector[0], 
                                orientation_estimate.vector[1], 
                                orientation_estimate.vector[2]);
    const float qw = orientation_estimate.scalar;
    
    const Eigen::Matrix3f term1 = skew_symmetric(q_vector);
    const Eigen::Matrix3f term2 = Eigen::Matrix3f::Identity() * qw;
    const Eigen::Matrix3f combined = term1 + term2;
    
    const Eigen::Vector3f vec_part = 0.5 * combined * gyro_meas_corrected;
    const float scalar_part = -0.5 * gyro_meas_corrected.dot(q_vector);
                                        
    orientation_dot = Quaternion(scalar_part, {vec_part(0), vec_part(1), vec_part(2)});

    // Euler integration
    orientation_estimate = (orientation_estimate + orientation_dot * time_delta).normalised();
    
    if(!is_static){
        position_estimate += velocity_estimate * time_delta;
        velocity_estimate += acc_world * time_delta;

        // Update the error state Jacobian blocks
        const Eigen::Matrix3f gyroSkew = skew_symmetric(gyro_meas_corrected);
        const Eigen::Matrix3f accelSkew = skew_symmetric(acc_meas_corrected);
        
        // G[0:3, 0:3] = -skewSymmetric(gyro_meas_corrected)
        G.block<3, 3>(0, 0) = -gyroSkew;
        
        // G[3:6, 0:3] = -R.dot(skewSymmetric(acc_meas_corrected))
        G.block<3, 3>(3, 0) = -R * accelSkew;
        
        // G[3:6, 12:15] = -R
        G.block<3, 3>(3, 12) = -R;

        // F = np.identity(15) + self.G * time_delta
        const Eigen::MatrixXf F = Eigen::MatrixXf::Identity(15, 15) + G * time_delta;
        
        // self.estimate_covariance = F.dot(self.estimate_covariance).dot(F.T) + self.process_covariance(time_delta)
        estimate_covariance = F * estimate_covariance * F.transpose() + process_covariance();
    }
    
}

void Kalman::update(const NavSensors& nav_sensors, const NavigationData& nav_data) {
    if(!is_static && nav_data.position.lat!=last_gps_lat && nav_data.position.lat!=last_gps_lon) {
        last_gps_lat = nav_data.position.lat;
        last_gps_lon = nav_data.position.lng;

        Eigen::Vector3f gps_meas;
        float alt_meas;
        gps_pressure_to_obs(nav_sensors, nav_data, gps_meas, alt_meas);

        Eigen::MatrixXf H = Eigen::MatrixXf::Zero(3, 15);
        
        // H[0:3, 6:9] = np.identity(3)
        H.block<3, 3>(0, 6) = Eigen::Matrix3f::Identity();
        
        // PH_T = self.estimate_covariance.dot(H.T)
        const Eigen::MatrixXf PH_T = estimate_covariance * H.transpose();
        
        // inn_cov = H.dot(PH_T) + self.observation_covariance
        const Eigen::Matrix3f inn_cov = H * PH_T + observation_covariance;
        
        // K = PH_T.dot(np.linalg.inv(inn_cov))
        const Eigen::MatrixXf K = PH_T * inn_cov.inverse();
        
        // observation = np.array([gps_meas[0], gps_meas[1], alt_meas])
        Eigen::Vector3f observation;
        observation << gps_meas(0), gps_meas(1), alt_meas;
        
        // innovation = observation - self.position_estimate
        const Eigen::Vector3f innovation = observation - position_estimate;
        
        // self.state = np.dot(K, innovation.transpose())
        state = K * innovation;
        
        // self.orientation_estimate = (self.orientation_estimate * Quaternion(scalar=1, vector=0.5 * self.state[0:3])).normalised
        const Eigen::Vector3f halfErrorVec = state.segment<3>(0) * 0.5f;
        const Quaternion errorQuat(1.0, {halfErrorVec(0), halfErrorVec(1), halfErrorVec(2)});
        orientation_estimate = (orientation_estimate * errorQuat).normalised();
        
        // Update estimates with state corrections
        velocity_estimate += state.segment<3>(3);
        position_estimate += state.segment<3>(6);
        gyro_bias += state.segment<3>(9);
        accelerometer_bias += state.segment<3>(12);
        
        // Reset state
        state.setZero();
        
        // self.estimate_covariance = (np.identity(15) - K.dot(H)).dot(self.estimate_covariance)
        estimate_covariance = (Eigen::MatrixXf::Identity(15, 15) - K * H) * estimate_covariance;
    }
}

Quaternion Kalman::get_orientation() const {
    return orientation_estimate;
}

Eigen::Vector3f Kalman::get_velocity() const {
    return velocity_estimate;
}

Eigen::Vector3f Kalman::get_position() const {
    return position_estimate;
}

Eigen::Vector3f Kalman::get_gyro_bias() const {
    return gyro_bias;
}

Eigen::Vector3f Kalman::get_accel_bias() const {
    return accelerometer_bias;
}

float Kalman::get_azimuth() const {
    return azimuth_of_quaternion(orientation_estimate);
}

float Kalman::get_pitch() const {
    return pitch_of_quaternion(orientation_estimate);
}
float Kalman::get_roll() const {
    return roll_of_quaternion(orientation_estimate);
}

NavigationData Kalman::get_nav_data() const {
    NavigationData nav_data;
    nav_data.position_kalman = {position_estimate(0), position_estimate(1), position_estimate(2)};
    nav_data.speed = {velocity_estimate(0), velocity_estimate(1), velocity_estimate(2)};
    nav_data.accel = current_accel;
    nav_data.attitude = {orientation_estimate.vector[0], orientation_estimate.vector[1], orientation_estimate.vector[2]};
    nav_data.altitude = position_estimate(2);
    return nav_data;
}