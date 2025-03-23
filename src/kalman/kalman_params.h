#ifndef KALMAN_PARAMS_H
#define KALMAN_PARAMS_H


// constants
#define HALF_PI 1.57079632679
#define M_PI 3.14159265358979323846
#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define EARTH_RADIUS 6378137.0

// depends on launching conditions
#define GRAVITY 9.807f // (for the IMUs) Change according to local gravity of launch
#define INITIAL_AZIMUTH 0.0f * DEG_TO_RAD // initial azimuth of the rocket (0° towards the north, 90° towards the east)

#define INITIAL_COV_GYR_BIAS 1e-6f // Initial covariance of the gyro bias    
#define INITIAL_COV_ACCEL_BIAS 1e-6f // Initial covariance of the accel bias
#define INITIAL_COV_ORIENTATION 1e-6f // Initial covariance of the orientation
#define GYRO_COV 1e-6f // Variance of the gyro noise (in rad/s)
#define GYRO_BIAS_COV 1e-6f // Variance of the gyro bias noise (in rad/s)
#define ACCEL_COV 1e-6f // Variance of the accel noise (in m/s^2)
#define ACCEL_BIAS_COV 1e-6f // Variance of the accel bias noise (in m/s^2)
#define GPS_OBS_COV 25.0f // Variance of the GPS (in meters) // TODO : separate lat and lon
#define ALT_OBS_COV 25.0f // Variance of the altimeter (in meters)

#endif // KALMAN_PARAMS_H