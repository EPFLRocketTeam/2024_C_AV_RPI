//
// Created by marin on 13/04/2024.
//

#include "../include/data/fakeSensors.h"

#include <iostream>
#include <string>
#include <utility>

FakeSensors::FakeSensors(){
    clean_data = SensFiltered();
}
FakeSensors::~FakeSensors()
= default;

void FakeSensors::calibrate()
{
    clean_data = SensFiltered();
}
bool FakeSensors::update()
{
    return true;
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
    /*
     * this is the format of the data:
    * altitude(m),velocity(m/s),Time(ms),acceleration,pressure(Pa)
    "165.0,0.0,-2463759.0," (0.1569064, 8.2768126, -5.687856999999999)",1005.710127303867"
     */

    //we want to extract the speed, acceleration,, altitude, and pressure
    //we will use the comma as a delimiter
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    sens.altitude = std::stod(data.substr(0, data.find(delimiter)));
    data.erase(0, data.find(delimiter) + delimiter.length());

    sens.speed.x = 0;
    sens.speed.y = 0;
    sens.speed.z = std::stod(data.substr(0, data.find(delimiter)));
    data.erase(0, data.find(delimiter) + delimiter.length());

    data.erase(0, data.find(delimiter) + delimiter.length());

    std::string accelString = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());
    //" (0.1569064, 8.2768126, -5.687856999999999)" this is the format of the acceleration
    // remove the first and last characters
    accelString = accelString.substr(2, accelString.size() - 3);
    std::cout << "accelString: " << accelString << std::endl;
    //we want to extract the x, y, and z components of the acceleration
    //we will use the comma as a delimiter
    std::string delimiter2 = ",";

    std::string token2;
    token2 = accelString.substr(0, accelString.find(delimiter2));
    sens.accel.x = std::stod(token2);
    accelString.erase(0, accelString.find(delimiter2) + delimiter2.length());

    sens.accel.y = std::stod(accelString.substr(0, accelString.find(delimiter2)));
    accelString.erase(0, accelString.find(delimiter2) + delimiter2.length());

    sens.accel.z = std::stod(accelString);

    data.erase(0, data.find(delimiter) + delimiter.length());
    sens.baro = std::stod(data);
    return sens;
}

void FakeSensors::set_data(std::string data)
{
    clean_data = parse_data(std::move(data));
}
