//
// Created by marin on 14/04/2024.
//

#ifndef THRESHOLDS_H
#define THRESHOLDS_H

/*
 * FLIGHT COMPUTER FSM THRESHOLDS
 * - - - - - - - - - - - - - - - 
 * All delays / timeouts are in milliseconds
 * All distance / altitude thresholds are in meters
 * ALl velocity thresholds are in meters per second
 * All pressure thresholds are in bar
 */

// Flight
#define G_GRAVITY_CST             9.80665 
#define ALTITUDE_ZERO             10
#define ALTITUDE_MAX              4000
#define ALTITUDE_THRESHOLD        300
#define SPEED_ZERO                0.1
#define ASCENT_MAX_DURATION_MS    30000
#define DESCENT_MAX_DURATION_MS    180e3
#define ACCEL_ZERO                0.1
#define ACCEL_LIFTOFF             0.1 * G_GRAVITY_CST
#define ACCEL_LIFTOFF_DURATION_MS 500
#define BURN_MAX_DURATION_MS      10000
// Moving averages
#define OVERPRESSURE_GRACE_MS  80
#define BUFFER_ACCEL_TIME_MS   500
#define MAX_LIFTOFF_TIMEOUT_MS 10000

// Pressurization
#define PRESSURIZATION_CHECK_PRESSURE 70
#define PRESSURIZATION_HOLD_MS        25400

// Ignition
#define IGNITION_NO_COM_TIMEOUT_MS 5000 // Timeout for PRB not receiving IGNITION command
#define IGNITION_FULL_DURATION_MS  5400 // From 2025_C_SE_FLIGHT_PARAMETERS
#define IGNITION_ACK_DELAY_MS      IGNITION_FULL_DURATION_MS + 50  // TO TUNE DEPENDING ON THE DELAY OF FSM TRANSITION 

// Passivation
#define PASSIVATION_DELAY_AFTER_APOGEE  10e3
#define PASSIVATION_DELAY_FOR_DPR       21e3
#define PASSIVATION_ALTITUDE_HIGH_BOUND 100

// ------LIMITS------
// Pressure limits in [bar]
#define PRESSURE_MIN      0.0f
#define TANK_PRESSURE_MAX 500.0f
#define TANK_PRESSURE_MIN PRESSURE_MIN
// Temperature limits in [°C]
#define TEMPERATURE_MIN   -273.15f
#define TEMPERATURE_MAX   500.0f

inline bool CHECK_TANK_PRESS(const float pressure) {
    return TANK_PRESSURE_MIN <= pressure && pressure <= TANK_PRESSURE_MAX;
}
inline bool CHECK_TEMPERATURE(const float temperature) {
    return TEMPERATURE_MIN <= temperature && temperature <= TEMPERATURE_MAX;
}

#endif //THRESHOLDS_H

