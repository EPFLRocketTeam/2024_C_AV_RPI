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

    std::string   eventPath;
    std::ofstream eventStream;

     // Singleton instance and mutex for thread safety
     static std::unique_ptr<DataLogger> instance;
     static std::mutex instanceMutex;
 
    DataLogger (std::string path, std::string eventPath);
public:

    // Deleted to prevent copying
    DataLogger(const DataLogger&) = delete;
    DataLogger& operator=(const DataLogger&) = delete;

    // Singleton accessor
    static DataLogger& getInstance(const std::string& path = "dump_log.txt", const std::string& eventPath = "event_log.txt") {
      
    }
    
    ~DataLogger ();
    void conv (DataDump &dump);
    void eventConv(std::string event,uint32_t ts);
};

#endif