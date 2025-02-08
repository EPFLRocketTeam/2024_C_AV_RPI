//
// Created by marin on 08.02.2025.
//
#include "data.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#ifndef FLIGHT_COMPUTER_DATALOGGER_H
#define FLIGHT_COMPUTER_DATALOGGER_H


class Datalogger {
public:
    Datalogger(std::string path,DataDump &dump);
    ~Datalogger();
    std::string conv(DataDump const &dump);
    int write(std::string row);

private:
    std::string path;
    std::ofstream csvFile;
};


#endif //FLIGHT_COMPUTER_DATALOGGER_H
