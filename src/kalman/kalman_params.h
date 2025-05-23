#ifndef KALMAN_PARAMS_H
#define KALMAN_PARAMS_H


// constants
#define HALF_PI 1.57079632679
#define M_PI 3.14159265358979323846
#define DEG_TO_RAD M_PI / 180.0f
#define RAD_TO_DEG 180.0f / M_PI
#define EARTH_RADIUS 6378137.0

// Positions of the accelerometters relative to the center of the PCB
// note : It would make more sense to put it relative to the center of mass of the rocket, but
//        the center of mass changes during the flight, and it would make tests less intuitive 
//        because rotating the card would lead to a velocity. Also it could lead to higher inaccuracies
//        because it is farther away from the IMUs
// positions are in the same frame as the imus (in meters)
#define ADXL_POS_X      -1.38f  / 1000.0f      // x position of the ADXL375
#define ADXL_POS_Y      30.94f  / 1000.0f      // y posiion of the ADXL375
#define ADXL_POS_Z      0.0f    / 1000.0f      // z position of the ADXL375
#define ADXL_AUX_POS_X  -12.98f / 1000.0f      // x position of the ADXL375 aux
#define ADXL_AUX_POS_Y  30.94f  / 1000.0f      // y position of the ADXL375 aux
#define ADXL_AUX_POS_Z  0.0f    / 1000.0f      // z position of the ADXL375 aux
#define BMI_POS_X       12.42f  / 1000.0f      // x position of the BMI088
#define BMI_POS_Y       30.94f  / 1000.0f      // y position of the BMI088
#define BMI_POS_Z       0.0f    / 1000.0f      // z position of the BMI088
#define BMI_AUX_POS_X   6.91f   / 1000.0f      // x position of the BMI088 aux
#define BMI_AUX_POS_Y   30.94f  / 1000.0f      // y position of the BMI088 aux
#define BMI_AUX_POS_Z   0.0f    / 1000.0f      // z position of the BMI088 aux

#define FUSED_IMU_POS_X  0
#define FUSED_IMU_POS_Y  0
#define FUSED_IMU_POS_Z  0

// depends on launching conditions
#define GRAVITY 9.807f // (for the IMUs) Change according to local gravity of launch
#define INITIAL_AZIMUTH 0.0f * DEG_TO_RAD // initial azimuth of the rocket (0° towards the north, 90° towards the east)

#define INITIAL_COV_GYR_BIAS    1e-6f // Initial covariance of the gyro bias    
#define INITIAL_COV_ACCEL_BIAS  1e-6f // Initial covariance of the accel bias
#define INITIAL_COV_ORIENTATION 1e-6f // Initial covariance of the orientation
#define GYRO_COV                1e-6f // Variance of the gyro noise (in rad/s)
#define GYRO_BIAS_COV           1e-6f // Variance of the gyro bias noise (in rad/s)
#define ACCEL_COV               1e-6f // Variance of the accel noise (in m/s^2)
#define ACCEL_BIAS_COV          1e-6f // Variance of the accel bias noise (in m/s^2)
#define GPS_OBS_COV             25.0f // Variance of the GPS (in meters) // TODO : separate lat and lon
#define ALT_OBS_COV             25.0f // Variance of the altimeter (in meters)

// Azimuth initialization method
#define USE_GPS_AZIMUTH_UPDATE true  // Set to true to update azimuth at first GPS update after takeoff
                                     // Set to false to use INITIAL_AZIMUTH directly

#endif // KALMAN_PARAMS_H