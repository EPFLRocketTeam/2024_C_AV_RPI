
#include "logger.h"
#include "data.h"

#include <iostream>

DataLogger::DataLogger (std::string path, std::string eventPath): path(path),eventPath(eventPath), stream(path),eventStream(eventPath) {}

DataLogger::~DataLogger () {
    stream.close();
    eventStream.close();
}

void DataLogger::conv (DataDump &dump) { 
    char* buffer = (char*) (&dump); 
    stream.write(buffer, sizeof(DataDump));
}

DataLogger::~DataLogger() {
    if (stream.is_open()) stream.close();
    if (eventStream.is_open()) eventStream.close();
}

DataLogger& DataLogger::getInstance(const std::string& path, const std::string& eventPath) {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (!instance) {
        instance = std::make_unique<DataLogger>(path);
    }
    return *instance;
}



void DataLogger::eventConv(std::string event,uint32_t ts){
    uint32_t str_length = event.size();
    eventStream.write((char*)ts, sizeof(ts));
    eventStream.write(event.c_str(), str_length*sizeof(char));
}
