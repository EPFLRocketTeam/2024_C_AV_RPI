#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <sstream>
#include "config.h"

using namespace std;
using namespace chrono;

const int CPU_THREADS = 4;        // Number of CPU stress threads
const int TEST_DURATION = 300;    // Max test time in seconds
const float MAX_TEMP = 80.0;      // Max temp before abort
const int LOG_INTERVAL = 5;       // Seconds between logs
const string LOG_PATH = "/boot/av_log/temp_log.csv";

atomic<bool> stop_flag(false);

// Simulated CPU load function
void cpu_stress_worker() {
    while (!stop_flag.load()) {
        volatile double x = 0.0;
        for (int i = 0; i < 100000; ++i) {
            x += sin(i) * tan(i); // heavy-ish FP ops
        }
    }
}

// Call vcgencmd and extract temperature
float get_temperature() {
    FILE* pipe = popen("sudo vcgencmd measure_temp", "r");
    if (!pipe) return -1;

    char buffer[128];
    fgets(buffer, sizeof(buffer), pipe);
    pclose(pipe);

    // Parse string like: temp=55.2'C
    string output(buffer);
    size_t start = output.find('=') + 1;
    size_t end = output.find('\'');
    string temp_str = output.substr(start, end - start);

    return stof(temp_str);
}

int main() {
    ofstream logfile(LOG_PATH, ios::out);
    if (!logfile) {
        cerr << "Failed to open log file: " << LOG_PATH << endl;
        return 1;
    }

    logfile << "Timestamp,Temperature(C)" << endl;

    // Launch CPU stress threads
    vector<thread> workers;
    for (int i = 0; i < CPU_THREADS; ++i) {
        workers.emplace_back(cpu_stress_worker);
    }

    auto start_time = steady_clock::now();

    while (true) {
        float temp = get_temperature();
        time_t now = time(nullptr);
        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

        logfile << timestamp << "," << temp << endl;
        logfile.flush();

        cout << "[" << timestamp << "] Temp: " << temp << " C" << endl;

        if (temp >= MAX_TEMP) {
            cout << "Temperature exceeded " << MAX_TEMP << "°C. Aborting test." << endl;
            break;
        }

        auto elapsed = steady_clock::now() - start_time;
        if (duration_cast<seconds>(elapsed).count() >= TEST_DURATION) {
            cout << "Test duration completed." << endl;
            break;
        }

        this_thread::sleep_for(seconds(LOG_INTERVAL));
    }

    stop_flag = true;

    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }

    logfile.close();
    return 0;
}