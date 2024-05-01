//
// Created by marin on 01/05/2024.
//

#include "../include/data/fakeSensors.h"
#include "fakeSensors.h"


FakeSensors::FakeSensors() : Sensors()
{
    // call the parent constructor implicitly
    // Sensors();
}

FakeSensors::~FakeSensors()
{
}

void FakeSensors::set_data(const std::string data)
{
    SensFiltered sens = parse_data(data);
}

bool FakeSensors::update()
{
    return true;
}

SensFiltered FakeSensors::parse_data(const std::string data)
{
    SensFiltered sens;

    //time(s),posx(m),posy(m),posz(m),vx(m/s),vy(m/s),vz(m/s),accx(m/s^2),accy(m/s^2),accz(m/s^2),rotx(rad/s),roty(rad/s),rotz(rad/s),pression(Pa),flight_phase, order_id, order_value

    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    std::string copy = std::move(data);
    for (int i = 0; i < 14; i++)
    {
        pos = copy.find(delimiter);


        token = copy.substr(0, pos);

        switch (i)
        {
        case 3:
            sens.altitude = std::stod(token);
            break;
        case 4:
            sens.speed.x = std::stod(token);
            break;
        case 5:
            sens.speed.y = std::stod(token);
            break;
        case 6:
            sens.speed.z = std::stod(token);
            break;
        case 7:
            sens.accel.x = std::stod(token);
            break;
        case 8:
            sens.accel.y = std::stod(token);
            break;
        case 9:
            sens.accel.z = std::stod(token);
            break;
        case 10:
            sens.attitude.x = std::stod(token);
            break;
        case 11:
            sens.attitude.y = std::stod(token);
            break;
        case 12:
            sens.attitude.z = std::stod(token);
            break;
        case 13:
            sens.baro = {std::stod(token),35};
            break;
        default:
            break;
        }
        copy = copy.erase(0, pos + 1);
    }

    return sens;
}



