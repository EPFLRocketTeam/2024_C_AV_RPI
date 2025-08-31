#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include "telecom.h"
#include "config.h"

static inline uint64_t ns_now() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

struct Stats {
    double avg, p50, p90, p99, max;
};

static Stats summarize(std::vector<double>& v) {
    if (v.empty()) return {};
    std::sort(v.begin(), v.end());
    double sum = 0;
    for (auto x : v) sum += x;
    auto at = [&](double q) { return v[(size_t)(q * (v.size()-1))]; };
    return {
        sum / v.size(),
        at(0.50), at(0.90), at(0.99), v.back()
    };
}

int main() {
    Telecom t;
    try {
        if (!t.begin()) {
            std::cerr << "Telecom.begin() failed\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Telecom.begin() threw: " << e.what() << "\n";
        return 1;
    }

    const int N = 2000;
    std::vector<double> times; times.reserve(N);

    for (int i=0; i<N; ++i) {
        auto t0 = ns_now();
        t.send_telemetry();
        auto t1 = ns_now();
        times.push_back((t1 - t0) / 1000.0); // us
    }

    auto s = summarize(times);
    std::cout << "send_telemetry() latency over " << N << " iterations:\n"
              << " avg=" << s.avg << "us"
              << " p50=" << s.p50 << "us"
              << " p90=" << s.p90 << "us"
              << " p99=" << s.p99 << "us"
              << " max=" << s.max << "us\n";

    for (int i=0; i<N; ++i) {
        auto t0 = ns_now();
        t.update();
        auto t1 = ns_now();
        times.push_back((t1 - t0) / 1000.0); // us
    }

    auto s = summarize(times);
    std::cout << "update() latency over " << N << " iterations:\n"
              << " avg=" << s.avg << "us"
              << " p50=" << s.p50 << "us"
              << " p90=" << s.p90 << "us"
              << " p99=" << s.p99 << "us"
              << " max=" << s.max << "us\n";

}
