#ifndef TSDB_H
#define TSDB_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>

class TimeSeriesDB {
public:
TimeSeriesDB() = default;
    ~TimeSeriesDB() = default;
    void loadCSV(const std::string& filename);
    std::optional<float> getValue(const int series, uint32_t timestamp) const;
private:
std::unordered_map<int, std::map<uint32_t, float>> data;
};

#endif /* TSDB_H */