#ifndef FAKESENSOR_H
#define FAKESENSOR_H

#include "sensors.h"


class FakeSensors : public Sensors {    
public:
    FakeSensors();
    ~FakeSensors();

    void set_raw_data(SensRaw data);
    void set_clean_data(SensFiltered data);
    void set_status(SensStatus status);
    void calibrate();
    bool update();
    SensStatus get_status() const;
    SensFiltered dump() const;
private:
    SensRaw raw_data;
    SensFiltered clean_data;
    SensStatus status;
};

#endif