
#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <optional>

struct TDB;

struct DataPoint {
    float time;
    std::vector<float> value;

    bool operator< (const DataPoint& other) const;
};

struct TimeSeries {
private:
    TDB* db;
    int  pt;
public:
    TimeSeries(TDB* db, int pt);
    
    float get();
};

struct TDB {
private:
    std::vector<DataPoint> _points;
    std::map<std::string, int> _mp;

    std::chrono::system_clock::time_point start;

    bool _init = false;
public:
    static TDB* from_csv (std::string path);

    void init ();

    int get_current_time ();
    DataPoint & get_current ();

    std::optional<TimeSeries> get_time_series (std::string name);
};
