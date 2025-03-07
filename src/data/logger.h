#ifndef LOGGER_H
#define LOGGER_H

#include "data.h"
#include <string>
#include <sstream>
#include <cstdint>
#include <fstream>
#include <vector>

class DataLogger {
private:
    std::string   path;
    std::ofstream stream;
public:
    DataLogger (std::string path);
    ~DataLogger ();
    void conv (DataDump &dump);
};

#endif