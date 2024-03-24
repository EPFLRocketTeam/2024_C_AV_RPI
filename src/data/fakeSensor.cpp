#include "../include/data/fakeSensor.h"




SensFiltered::SensFiltered()
:   speed{0, 0, 0},
    accel{0, 0, 0},
    attitude{0, 0, 0},
    baro_atm{0, 0},
    N2_pressure(0),
    fuel_pressure(0),
    LOX_pressure(0),
    fuel_level(0),
    LOX_level(0),
    engine_temperature(0),
    igniter_pressure(0),
    LOX_inj_pressure(0),
    fuel_inj_pressure(0),
    chamber_pressure(0)
{}