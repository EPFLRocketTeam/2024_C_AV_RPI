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
#define ACCEL_ZERO                0.1
#define ACCEL_LIFTOFF             0.1 * G_GRAVITY_CST
#define ACCEL_LIFTOFF_DURATION_MS 500
// Moving averages
#define OVERPRESSURE_GRACE_MS  80
#define BUFFER_ACCEL_TIME_MS   500
#define MAX_LIFTOFF_TIMEOUT_MS 10000

// Pressurization
#define PRESSURIZATION_CHECK_PRESSURE 70
#define PRESSURIZATION_HOLD_MS        10400 + 5000

// Ignition
#define IGNITION_NO_COM_TIMEOUT_MS 5000
#define IGNITION_ACK_DELAY_MS      4600  // TO TUNE DEPENDING ON THE ACTUAL DELAY ON IGNITION SEQUENCE

// Passivation
#define PASSIVATION_DELAY_AFTER_APOGEE  90e3
#define PASSIVATION_DELAY_FOR_DPR       PASSIVATION_DELAY_AFTER_APOGEE + 30e3
#define PASSIVATION_ALTITUDE_HIGH_BOUND 100

#endif //THRESHOLDS_H

