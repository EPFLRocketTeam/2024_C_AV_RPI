//
// Created by marin on 30/04/2024.
//

#ifndef FAKESENSORS_H
#define FAKESENSORS_H

//Fake sensors inherits from Sensors
#include "data/sensors.h"

class FakeSensors : public Sensors
{
    public:
        FakeSensors();
        ~FakeSensors();
    void set_data(const std::string data);
    //override update method
    
    SensFiltered parse_data(const std::string data);

    bool update() override;
private:
    SensFiltered m_data;
    SensRaw m_raw;

};

#endif //FAKESENSORS_H
