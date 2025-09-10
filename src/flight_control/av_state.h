// This is the header file for the AvState class, which is a part of the flightControl module.
// The AvState class is a finite state machine (FSM) that represents the state of the rocket.
// The class has a constructor and a destructor, as well as a number of functions that transition from one state to others possible states.

#ifndef AVSTATE_H
#define AVSTATE_H

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "thresholds.h"
#include "data.h"

class MovingAverage {
public:
// power
    MovingAverage(size_t power);
    void addSample(float sample);
    float getAverage() const;
    void reset();
protected:
    std::vector<float> samples;
    size_t maxSize;
    float sum;
};

class WeightedMovingAverage : public MovingAverage {
public:
    explicit WeightedMovingAverage(size_t power);
    void addSample(float sample);
    float getAverage() const; // (optional) override if you want weighted avg out

private:
    std::vector<float> weights;
    float weighted_sum = 0.0f;
    float total_weight = 0.0f;
};


// Path: AV-Firehorn-Rpi/include/flightControl/AvState.h
// Compare this snippet from AV-Firehorn-Rpi/src/flightControl/FSM.cpp:
//     // this function allows to get the current state of the FSM
// functions that transition from one state to others possible states
class AvState
{
public:
    // constructor
    explicit AvState();
    // destructor
    ~AvState();
    // this function allows to get the current state of the FSM
    State getCurrentState();
    void update(const DataDump &dump,uint32_t delat_ms);
    std::string stateToString(State state);

private:
    void reset_flight();
    State from_init(DataDump const &dump,uint32_t delta_ms);
    State from_calibration(DataDump const &dump,uint32_t delta_ms);
    State from_filling(DataDump const &dump,uint32_t delta_ms);
    State from_armed(DataDump const &dump,uint32_t delta_ms);
    State from_pressurization(DataDump const &dump,uint32_t delta_ms);
    State from_abort_ground(DataDump const &dump,uint32_t delta_ms);
    State from_ignition(DataDump const &dump,uint32_t delta_ms);
    State from_landed(DataDump const &dump,uint32_t delta_ms);
    State from_burn(DataDump const &dump,uint32_t delta_ms);
    State from_ascent(DataDump const &dump,uint32_t delta_ms);
    State from_descent(DataDump const &dump,uint32_t delta_ms);
    State from_abort_flight(DataDump const &dump,uint32_t delta_ms);
    State currentState;

    MovingAverage pressure_fuel_avg{6}; // 64 samples
    MovingAverage pressure_lox_avg{6}; // 64 samples
                                       //
    uint32_t pressurization_start_time;
    uint32_t timer_accel;
    uint32_t timer_liftoff_timeout;
    
    WeightedMovingAverage altitude_avg{4}; // 16 samples
    WeightedMovingAverage lagged_alt_avg{4}; // 16 samples
    std::vector<float> bmp_buffer;
    std::vector<uint32_t> ts_buffer;

    uint32_t ascent_elapsed;
    uint32_t descent_elapsed;
};


#endif
