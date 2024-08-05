//
// Created by marin on 14/04/2024.
//

#ifndef THRESHOLDS_H
#define THRESHOLDS_H

class Thresholds
{
    // a class that contains the thresholds for the different data values
    // this class is used to hold the thresholds for the different data values

public:
    Thresholds( double speed_zero,
                double altitude_max,
                double altitude_zero,
                double lox_pressure_wanted,
                double lox_pressure_zero,
                double fuel_pressure_wanted,
                double fuel_pressure_zero,
                double chamber_pressure_wanted,
                double chamber_pressure_zero,
                double injector_pressure_wanted_min,
                double injector_pressure_wanted_max,
                double engine_temp_zero,
                double engine_temp_wanted,
                double fuel_inj_zero,
                double lox_level_zero,
                double fuel_level_zero,
                double lox_level_full,
                double fuel_level_full,
                double n2_pressure_zero,
                double igniter_pressure_wanted);

    Thresholds();
    ~Thresholds();

    const double speed_zero;
    const double altitude_max;
    const double altitude_zero;
    const double lox_pressure_wanted;
    const double lox_pressure_zero;
    const double fuel_pressure_wanted;
    const double fuel_pressure_zero;
    const double chamber_pressure_wanted;
    const double chamber_pressure_zero;
    const double engine_temp_zero;
    const double engine_temp_wanted;
    const double injector_pressure_wanted_min;
    const double injector_pressure_wanted_max;
    const double fuel_inj_zero;
    const double lox_level_zero;
    const double fuel_level_zero;
    const double lox_level_full;
    const double fuel_level_full;
    const double n2_pressure_zero;
    const double igniter_pressure_wanted;

};
#endif //THRESHOLDS_H
