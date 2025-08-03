
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <filesystem>
#include <exception>
#include "logger.h"
#include "data.h"
#include "config.h"

// If the folder contains more than 10^6 files, throw an error
const int MAX_TEMPLATE_COUNT = 1'000'000;

namespace {
    std::string template_based_path(std::string path);
    std::string severity_to_str(const Logger::Severity lvl);
    // Log files path
    std::string av_log_dump_path(template_based_path(LOG_DUMP_DEFAULT_PATH));
    std::string av_log_event_path(template_based_path(LOG_EVENT_DEFAULT_PATH));
    int dump_fd;
    int event_fd;
}


bool Logger::init() {
    dump_fd = open(av_log_dump_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0777);
    event_fd = open(av_log_event_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (dump_fd == -1) {
        // throw DataLoggerException("Could not open the dump file.\n");
        return false;
    }
    if (event_fd == -1) {
        // throw DataLoggerException("Could not open the event file.\n");
        return false;
    }
    return true;
}

bool Logger::init(const std::string dump_path, const std::string event_path) {
    av_log_dump_path = template_based_path(dump_path);
    av_log_event_path = template_based_path(event_path);
    dump_fd = open(av_log_dump_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0777);
    event_fd = open(av_log_event_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (dump_fd == -1) {
        // throw DataLoggerException("Could not open the dump file.\n");
        return false;
    }
    if (event_fd == -1) {
        // throw DataLoggerException("Could not open the event file.\n");
        return false;
    }
    return true;
}

void Logger::terminate() {
    fsync(dump_fd);
    fsync(event_fd);

    close(dump_fd);
    close(event_fd);
}

void Logger::log_dump(const DataDump& dump) {
    static int counter(0);
    DataDump dump_copy(dump);
    char* buffer = reinterpret_cast<char*>(&dump_copy);
    write(dump_fd, buffer, sizeof(DataDump));
    
    counter++;

    if (counter >= DATADUMP_FSYNC_FREQ){
        counter = 0;
        fsync(dump_fd);
    }
}

void Logger::log_event(const Severity lvl, const std::string event) {
    const std::string event_msg(severity_to_str(lvl) + event);
    const uint32_t str_length = event_msg.size();

    const uint32_t timestamp(Data::get_instance().get().av_timestamp);
    char* buffer_ts = (char*) &timestamp;
    char* buffer_ln = (char*) &str_length;

    write(event_fd, buffer_ts, sizeof(timestamp));
    write(event_fd, buffer_ln, sizeof(uint32_t));
    write(event_fd, event_msg.c_str(), str_length * sizeof(char));

    fsync(event_fd);
}

std::string Logger::get_dump_path() {
    return av_log_dump_path;
}

std::string Logger::get_event_path() {
    return av_log_event_path;
}

namespace {
    std::string template_based_path(std::string path) {
        int base_sl = path.find_last_of('/');
        int ext_dot = path.find_last_of('.');
        
        bool uses_extension = ext_dot != -1 && ext_dot > base_sl;

        std::string base = uses_extension ? path.substr(0, ext_dot) : path;
        std::string suff = uses_extension ? path.substr(ext_dot) : "";
        for (int i = 0; i < MAX_TEMPLATE_COUNT; i ++) {
            std::string target = base + "_" + std::to_string(i) + suff;
            if (std::filesystem::exists(target)) {
                continue;
            }
            return target;
        }

        std::cout << "Could not start the logger as all the " << MAX_TEMPLATE_COUNT
                  << " files exist.\n";
        return "";
    }

    std::string severity_to_str(const Logger::Severity lvl) {
        switch (lvl) {
            case Logger::FATAL:
                return "[FATAL]: ";
            case Logger::ERROR:
                return "[ERROR]: ";
            case Logger::WARN:
                return "[WARN]: ";
            case Logger::INFO:
                return "[INFO]: ";
            case Logger::DEBUG:
                return "[DEBUG]: ";
            default:
                return "";
        }
    }
}

// DataLogger::DataLogger(std::string _dumpPath, std::string _eventPath) {
//     dumpFd  = open(dumpPath.c_str(),  O_WRONLY | O_CREAT |  O_APPEND, 0777);
//     eventFd = open(eventPath.c_str(), O_WRONLY | O_CREAT |  O_APPEND, 0777);
    
//     if (dumpFd == -1) {
//         throw std::runtime_error("Could not open the dump file.\n");
//     }
//     if (eventFd == -1) {
//         throw std::runtime_error("Could not open the event file.\n");
//     }
// }


// void DataLogger::conv(DataDump &dump) { 
//     static int counter(0);
//     char* buffer = reinterpret_cast<char*>(&dump);
//     write(dumpFd, buffer, sizeof(DataDump));
    
//     counter++;

//     if (counter >= DATADUMP_FSYNC_FREQ){
//         counter = 0;
//         fsync(dumpFd);
//     }
// }

// DataLogger::~DataLogger() {
//     fsync(dumpFd);
//     fsync(eventFd);

//     close(dumpFd);
//     close(eventFd);
// }

// DataLogger& DataLogger::getInstance(const std::string& path, const std::string& eventPath) {
//     std::lock_guard<std::mutex> lock(instanceMutex);  
//      if (!instance) {
//         instance = std::make_unique<DataLogger>(path,eventPath);
//     }
//     return *instance;
// }

// void DataLogger::eventConv(std::string event,uint32_t ts){
//     uint32_t str_length = event.size();

//     char* buffer_ts = (char*) &ts;
//     char* buffer_ln = (char*) &str_length;

//     write(eventFd, buffer_ts, sizeof(ts));
//     write(eventFd, buffer_ln, sizeof(uint32_t));
//     write(eventFd, event.c_str(), str_length * sizeof(char));

//     fsync(eventFd);
// }