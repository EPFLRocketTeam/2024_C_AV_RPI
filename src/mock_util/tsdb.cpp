#include "tsdb.h"


TimeSeriesDB::TimeSeriesDB(){
    data.clear();
}


void TimeSeriesDB::loadCSV(const std::string& filename) {
    std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        std::string line;
        // skip header if present
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name, valueStr, tsStr;
            if (!std::getline(ss, name, ',')) continue;
            if (!std::getline(ss, valueStr, ',')) continue;
            if (!std::getline(ss, tsStr, ',')) continue;

            float value = std::stof(valueStr);
            uint64_t ts = std::stoull(tsStr);
            int id = std::stoi(name);
            data[id][ts] = value;
        }
    }

    std::optional<float> TimeSeriesDB::getValue(const int series, uint32_t timestamp) const {
        auto it = data.find(series);
        if (it == data.end()) {
            return std::nullopt; // Series not found
        }

        const auto& seriesData = it->second;
        auto tsIt = seriesData.find(timestamp);
        if (tsIt == seriesData.end()) {
            return std::nullopt; // Timestamp not found
        }

        return tsIt->second;
    }