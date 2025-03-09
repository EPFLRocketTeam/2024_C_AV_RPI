
#include "logger.h"
#include "data.h"

#include <iostream>

DataLogger::DataLogger (std::string path): path(path), stream(path) {}

DataLogger::~DataLogger () {
    stream.close();
}

void DataLogger::conv (DataDump &dump) { 
    char* buffer = (char*) (&dump); 
    stream.write(buffer, sizeof(DataDump));
}
