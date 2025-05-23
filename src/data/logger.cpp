
#include "logger.h"
#include "data.h"
#include "config.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <filesystem>
#include <exception>

// If the folder contains more than 10^6 files, throw an error
const int MAX_TEMPLATE_COUNT = 1'000'000;

std::string template_based_path (std::string path) {
    int base_sl = path.find_last_of('/');
    int ext_dot = path.find_last_of('.');
    
    bool uses_extension = ext_dot != -1 && ext_dot > base_sl;

    std::string base = uses_extension ? path.substr(0, ext_dot) : path;
    std::string suff = uses_extension ? path.substr(ext_dot) : "";
    for (int i = 0; i < MAX_TEMPLATE_COUNT; i ++) {
        std::string target = base + "_" + std::to_string(i) + suff;
        if (std::filesystem::exists(target)) continue ;

        return target;
    }

    std::cout << "Could not start the logger as all the " << MAX_TEMPLATE_COUNT << " files exist.\n";
    exit(1);
    return "";
}

DataLogger::DataLogger (std::string _dumpPath, std::string _eventPath) {
    dumpPath  = template_based_path(_dumpPath);
    eventPath = template_based_path(_eventPath);

    dumpFd  = open(dumpPath.c_str(),  O_BINARY | O_WRONLY | O_CREAT |  O_APPEND);
    eventFd = open(eventPath.c_str(), O_BINARY | O_WRONLY | O_CREAT |  O_APPEND);
    
    if (dumpFd == -1) {
        throw std::runtime_error("Could not open the dump file.\n");
    }
    if (eventFd == -1) {
        throw std::runtime_error("Could not open the event file.\n");
    }
}

std::unique_ptr<DataLogger> DataLogger::instance;
std::mutex DataLogger::instanceMutex;


void DataLogger::conv (DataDump &dump) { 
    static int counter =0;
    char* buffer = reinterpret_cast<char*>(&dump);
    write(dumpFd, buffer, sizeof(DataDump));
    
    counter++;

    if (counter >= DATADUMP_FSYNC_FREQ){
        counter = 0;
        fsync(dumpFd);
    }
}

DataLogger::~DataLogger() {
    fsync(dumpFd);
    fsync(eventFd);

    close(dumpFd);
    close(eventFd);
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

    char* buffer_ts = (char*) &ts;
    char* buffer_ln = (char*) &str_length;

    write(eventFd, buffer_ts, sizeof(ts));
    write(eventFd, buffer_ln, sizeof(uint32_t));
    write(eventFd, event.c_str(), str_length * sizeof(char));

    fsync(eventFd);
}

std::string DataLogger::getDumpPath  () { return dumpPath; }
std::string DataLogger::getEventPath () { return eventPath; }