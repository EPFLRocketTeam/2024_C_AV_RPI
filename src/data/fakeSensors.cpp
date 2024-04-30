//
// Created by marin on 13/04/2024.
//

#include "../include/data/fakeSensors.h"

#include <cstring>
#include <iostream>
#include <string>
#include <utility>

FakeSensors::FakeSensors()
{
    clean_data = SensFiltered();
    status = SensStatus();
    //init all status to 0
    status.adxl_status = 0;
    status.adxl_aux_status = 0;
    status.bmi_status = 0;
    status.bmi_aux_status = 0;
}

FakeSensors::~FakeSensors()
= default;

void FakeSensors::calibrate()
{
    clean_data = SensFiltered();
}

bool FakeSensors::update(std::string data)
{
    set_data(data);
    return true;
}

void FakeSensors::update_status()
{
    //set all to 1
    status.adxl_status = 1;
    status.adxl_aux_status = 1;
    status.bmi_status = 1;
    status.bmi_aux_status = 1;
}

SensFiltered::SensFiltered()
{
    speed.x = 0;
    speed.y = 0;
    speed.z = 0;

    accel.x = 0;
    accel.y = 0;
    accel.z = 0;

    attitude.x = 0;
    attitude.y = 0;
    attitude.z = 0;

    baro = 0;
    N2_pressure = 0;
    fuel_pressure = 0;
    LOX_pressure = 0;
    fuel_level = 0;
    LOX_level = 0;
    engine_temperature = 0;
    igniter_pressure = 0;
    LOX_inj_pressure = 0;
    fuel_inj_pressure = 0;
    chamber_pressure = 0;
    altitude = 0;
}

SensFiltered parse_data(std::string data)
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
            sens.baro = std::stod(token);
            break;
        default:
            break;
        }
        copy = copy.erase(0, pos + 1);
    }

    return sens;
}


void FakeSensors::set_data(std::string data)
{
    clean_data = parse_data(data);
}
