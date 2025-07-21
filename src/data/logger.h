#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <sstream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <memory>
#include <mutex>

struct DataDump;

class DataLogger {    
public:
    DataLogger(std::string dumpPath, std::string eventPath);
    
    // Deleted to prevent copying
    DataLogger(const DataLogger&) = delete;
    DataLogger& operator=(const DataLogger&) = delete;

    // Singleton accessor
    static DataLogger& getInstance(const std::string& path = "/boot/av_log/dump_log.log", const std::string& eventPath = "/boot/av_log/event_log.log");
    
    ~DataLogger();
    void conv(DataDump &dump);
    void eventConv(std::string event,uint32_t ts);

    template <typename ...Args>
    void eventConvf (const char* fmt, uint32_t ts, Args&&... args) {
        int size = std::snprintf(NULL, 0, fmt, args...);
        if (size < 0) {
            throw std::exception();
        }

        char* buffer = (char*) malloc((size + 1) * sizeof(char));

        std::snprintf(buffer, size + 1, fmt, args...);

        std::string result(size, '.');
        for (size_t off = 0; off < size; off++)
            result[off] = buffer[off];

        free(buffer);

        eventConv(result, ts);
    }

    inline std::string getDumpPath() const { return dumpPath; }
    inline std::string getEventPath() const { return eventPath; }

private:
    std::string dumpPath;
    std::string eventPath;

    int dumpFd;
    int eventFd;

     // Singleton instance and mutex for thread safety
     static std::unique_ptr<DataLogger> instance;
     static std::mutex instanceMutex;
};

#endif /* LOGGER_H */