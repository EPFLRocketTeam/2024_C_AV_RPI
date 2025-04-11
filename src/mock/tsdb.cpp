
#include <algorithm>
#include "tsdb.h"
#include "unistd.h"
#include <iostream>
#include <assert.h>

bool DataPoint::operator<(const DataPoint &other) const
{
    return time < other.time;
}

void TDB::init()
{
    if (_init)
        return;
    _init = true;

    sort(_points.begin(), _points.end());
    start = std::chrono::system_clock::now();
}

int TDB::get_current_time()
{
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> delta_seconds = end - start;
    double delta = delta_seconds.count();

    int a = 0;
    int b = _points.size();

    while (b - a > 1)
    {
        int m = (a + b) >> 1;

        if (_points[m].time <= delta)
            a = m;
        else
            b = m;
    }

    return a;
}
DataPoint &TDB::get_current()
{
    return _points[get_current_time()];
}

std::optional<TimeSeries> TDB::get_time_series(std::string name)
{
    auto it = _mp.find(name);
    if (it == _mp.end())
        return {};

    int tar = (*it).second;

    TimeSeries series(this, tar);
    return {series};
}

TimeSeries::TimeSeries(TDB *db, int pt) : db(db), pt(pt) {}
float TimeSeries::get()
{
    return db->get_current().value[pt];
}

std::vector<std::string> split(std::string &string, char chr)
{
    std::vector<std::string> res;

    int start = 0;
    for (int i = 0; i <= string.size(); i++)
    {
        bool is_end = i == string.size() || string[i] == chr;

        if (is_end)
        {
            if (start == i)
                res.push_back("");
            else
                res.push_back(string.substr(start, i - start));
            start = i + 1;
        }
    }

    return res;
}
TDB *TDB::from_csv(std::string path)
{
    FILE *file = fopen(path.c_str(), "r");
    if (file == nullptr)
    {
        std::cout << "Could not open file\n";
        return nullptr;
    }

    std::string str;
    while (1)
    {
#define _BUFFER_SIZE 1024
        char buffer[_BUFFER_SIZE];
        size_t res = fread(buffer, 1, _BUFFER_SIZE, file);

        if (res > 0)
        {
            std::string lcl(res, '.');

            for (int i = 0; i < res; i++)
                lcl[i] = buffer[i];

            str += lcl;
        }

        if (res < _BUFFER_SIZE)
            break;
    }

    std::vector<std::string> lines = split(str, '\n');

    std::vector<std::string> words_first_line = split(lines[0], ',');
    if (words_first_line[0] != "time")
    {
        std::cout << "First column should be \"time\"\n";
        return nullptr;
    }

    std::map<std::string, int> _mp;
    for (int i = 1; i < words_first_line.size(); i++)
        _mp[words_first_line[i]] = i - 1;
    std::vector<DataPoint> points;

    for (int iline = 1; iline < lines.size(); iline++)
    {
        std::string &line = lines[iline];
        if (line.empty())
            continue;
        std::vector<std::string> words = split(line, ',');
        if (words.size() < 1)
            continue;

        try
        {
            float time = std::stof(words[0]);

            std::vector<float> values;
            for (int iword = 1; iword < words.size(); iword++)
            {
                try
                {
                    float value = std::stof(words[iword]);
                    values.push_back(value);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Invalid value in line " << iline + 1 << ", column " << iword + 1 << ": " << words[iword] << "\n";
                    values.push_back(0.0);
                }
            }

            DataPoint point;
            point.time = time;
            point.value = values;

            points.push_back(point);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Invalid time in line " << iline + 1 << ": " << words[0] << "\n";
        }
    }

    TDB *tdb = new TDB();
    tdb->_mp = _mp;
    tdb->_points = points;

    return tdb;
}
