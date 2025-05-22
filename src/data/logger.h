#ifndef LOGGER_H
#define LOGGER_H

#include "data.h"
#include <string>
#include <sstream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <memory>
#include <mutex>

class DataLogger {
private:
    std::string   path;
    std::ofstream stream;
    int fd;

    std::string   eventPath;
    std::ofstream eventStream;
    int fdStream;

     // Singleton instance and mutex for thread safety
     static std::unique_ptr<DataLogger> instance;
     static std::mutex instanceMutex;
 
    
public:
DataLogger (std::string path, std::string eventPath);
    // Deleted to prevent copying
    DataLogger(const DataLogger&) = delete;
    DataLogger& operator=(const DataLogger&) = delete;

    // Singleton accessor
    static DataLogger& getInstance(const std::string& path = "/boot/av_log/dump_log.log", const std::string& eventPath = "/boot/av_log/event_log.log");
    
    ~DataLogger ();
    void conv (DataDump &dump);
    void eventConv(std::string event,uint32_t ts);

    template<typename... Args>
    void eventConvf (const char* fmt, uint32_t ts, Args&&... args);

    std::string getPath ();
    std::string getEventPath ();
};

#endif