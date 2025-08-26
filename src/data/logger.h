#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <cstdint>
#include <exception>
#include "config.h"

struct DataDump;

namespace Logger {
    enum Severity {
        FATAL,
        ERROR,
        WARN,
        INFO,
        DEBUG,
        NB_SEVERITY
    };

    bool init();
    bool init(const std::string dump_path, const std::string event_path);
    void terminate();
    void log_dump(const DataDump& dump);
    void log_event(const Severity lvl, const std::string event);

    // Default log severity is INFO
    template <typename ...Args>
    inline void log_eventf(const char* fmt, Args&&... args) {
        const int size = std::snprintf(NULL, 0, fmt, args...);
        if (size < 0) {
            return;
        }

        char* buffer = (char*) malloc((size + 1) * sizeof(char));
        std::snprintf(buffer, size + 1, fmt, args...);

        std::string result(size, '.');
        for (size_t off = 0; off < size; off++) {
            result[off] = buffer[off];
        }

        free(buffer);
        log_event(INFO, result);
    }

    template <typename ...Args>
    inline void log_eventf(Severity lvl, const char* fmt, Args&&... args) {
#if !(DEBUG_LOG)
        if (lvl == DEBUG) {
            return;
        }
#endif

        const int size = std::snprintf(NULL, 0, fmt, args...);
        if (size < 0) {
            return;
        }

        char* buffer = (char*) malloc((size + 1) * sizeof(char));
        std::snprintf(buffer, size + 1, fmt, args...);

        std::string result(size, '.');
        for (size_t off = 0; off < size; off++) {
            result[off] = buffer[off];
        }

        free(buffer);
        log_event(lvl, result);
    }
    
    std::string get_dump_path();
    std::string get_event_path();
}

// class DataLoggerException : public std::exception {
// public:
//     DataLoggerException(const std::string msg_) : msg(msg_) {}

//     virtual const char* what() const throw() {
//         return msg.c_str();
//     }

// private:
//     std::string msg;
// };

// class DataLogger {    
// public:
//     DataLogger(std::string dumpPath, std::string eventPath);
    
//     // Deleted to prevent copying
//     DataLogger(const DataLogger&) = delete;
//     DataLogger& operator=(const DataLogger&) = delete;

//     Singleton accessor
//     static DataLogger& getInstance(const std::string& path = "/boot/av_log/dump_log.log", const std::string& eventPath = "/boot/av_log/event_log.log");
    
//     ~DataLogger();
//     void conv(DataDump &dump);
//     void eventConv(std::string event,uint32_t ts);

//     template <typename ...Args>
//     void eventConvf (const char* fmt, uint32_t ts, Args&&... args) {
//         int size = std::snprintf(NULL, 0, fmt, args...);
//         if (size < 0) {
//             // throw std::exception();
//             return;
//         }

//         char* buffer = (char*) malloc((size + 1) * sizeof(char));

//         std::snprintf(buffer, size + 1, fmt, args...);

//         std::string result(size, '.');
//         for (size_t off = 0; off < size; off++)
//             result[off] = buffer[off];

//         free(buffer);

//         eventConv(result, ts);
//     }

// private:

//     int dumpFd;
//     int eventFd;
// };

#endif /* LOGGER_H */
