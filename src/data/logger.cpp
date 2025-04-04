
#include "logger.h"
#include "data.h"
#include <unistd.h>
#include <iostream>

DataLogger::DataLogger (std::string path, std::string eventPath): path(path),eventPath(eventPath), stream(path),eventStream(eventPath) {}

std::unique_ptr<DataLogger> DataLogger::instance;
std::mutex DataLogger::instanceMutex;


void DataLogger::conv (DataDump &dump) { 
static int counter =0;
    char* buffer = reinterpret_cast<char*>(&dump);
    stream.write(buffer, sizeof(DataDump));
    if (__glibc_unlikely((counter & 63 )!=0)){
        counter =0;
        ::fsync(fd);

    }
    counter++;


}

DataLogger::~DataLogger() {
    stream.flush();
    eventStream.flush();
    ::fsync(fd);::fsync(fd);
    ::close(fdStream);
   ::close(fdStream);

    if (stream.is_open()) stream.close();
    if (eventStream.is_open()) eventStream.close();
}

DataLogger& DataLogger::getInstance(const std::string& path, const std::string& eventPath) {
    std::lock_guard<std::mutex> lock(instanceMutex);  
     if (!instance) {
        instance = std::make_unique<DataLogger>(path,eventPath);
    }
    return *instance;
}



void DataLogger::eventConv(std::string event,uint32_t ts){
    uint32_t str_length = event.size();
    eventStream.write(reinterpret_cast<char*>(&ts), sizeof(ts));
    eventStream.write(reinterpret_cast<char*>(&str_length), sizeof(uint32_t));
    eventStream.write(event.c_str(), str_length*sizeof(char));
    stream.flush();
    eventStream.flush();
   ::fsync(fd);
   ::fsync(fdStream);

}
