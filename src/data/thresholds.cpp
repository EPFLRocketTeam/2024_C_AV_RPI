//
// Created by marin on 14/04/2024.
//

#include "data/thresholds.h"

Thresholds::Thresholds(const double speed_zero, const double speed_max, const double altitude_max, const double acceleration_max, const double pressure_max, const double pressure_wanted, const double pressure_min)
{
    this->speed_zero = speed_zero;
    this->speed_max = speed_max;
    this->altitude_max = altitude_max;
    this->acceleration_max = acceleration_max;
    this->pressure_max = pressure_max;
    this->pressure_wanted = pressure_wanted;
    this->pressure_min = pressure_min;
}

Thresholds::Thresholds()
{
    this->speed_zero = 0.0;
    this->speed_max = 0.0;
    this->altitude_max = 0.0;
    this->acceleration_max = 0.0;
    this->pressure_max = 0.0;
    this->pressure_wanted = 0.0;
    this->pressure_min = 0.0;
}

Thresholds::~Thresholds()
= default;
