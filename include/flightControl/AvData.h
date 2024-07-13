#include <string.h>
#include <string>
#ifndef AVDATA_H
#define AVDATA_H

class AvData{
    public:
        float pressure;
        float temperature;
        float altitude;
        float acceleration;
        float velocity;
        float thrust;
        float battery;
        float voltage;
        float current;
        float power;
        float energy;
        float capacity;
        float efficiency;
        float time;
       
        std::string telemetry;
        bool ignited;
 
};

#endif