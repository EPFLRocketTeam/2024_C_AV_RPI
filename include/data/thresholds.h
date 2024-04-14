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
    Thresholds(const double speed_zero, const double speed_max, const double altitude_max, const double acceleration_max, const double pressure_max, const double pressure_wanted, const double pressure_min);
    Thresholds();

    ~Thresholds();
    double speed_zero;
    double speed_max;
    double altitude_max;
    double acceleration_max;
    double pressure_max;
    double pressure_wanted;
    double pressure_min;
};
#endif //THRESHOLDS_H
