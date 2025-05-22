
#include "logger.h"
#include "data.h"
#include <unistd.h>
#include <iostream>
#include <filesystem>

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

DataLogger::DataLogger (std::string _path, std::string _eventPath) {
    path = template_based_path(_path);
    eventPath = template_based_path(eventPath);
    stream = std::ofstream(path, std::ios::binary | std::ios::app);
    eventStream = std::ofstream(eventPath, std::ios::binary | std::ios::app);
}

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

template <typename ...Args>
void DataLogger::eventConvf (const char* fmt, uint32_t ts, Args&&... args) {
    int size = std::snprintf(NULL, 0, fmt, args...);
    if (size < 0) {
        throw std::exception();
    }

    char* buffer = (char*) malloc( (size + 1) * sizeof(char) );

    std::snprintf(buffer, size + 1, fmt, args...);

    std::string result(size, '.');
    for (size_t off = 0; off < size; off ++)
        result[off] = buffer[off];

    free(buffer);

    eventConv(result, ts);
}

std::string DataLogger::getPath      () { return path; }
std::string DataLogger::getEventPath () { return eventPath; }