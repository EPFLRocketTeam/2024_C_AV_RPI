#include <thread>
#include "tsdb.h"
#include <cassert>
int main () {
    TDB* tdb = TDB::from_csv("example.csv");

    TimeSeries metric1 = tdb->get_time_series("metric1").value();
    TimeSeries metric2 = tdb->get_time_series("metric2").value();
    TimeSeries metric3 = tdb->get_time_series("metric3").value();
    assert( !tdb->get_time_series("metric4").has_value() );

    tdb->init();

    assert(metric1.get() == 1.0);
    assert(metric2.get() == 2.0);
    assert(metric3.get() == 3.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    assert(metric1.get() == 1.0);
    assert(metric2.get() == 2.0);
    assert(metric3.get() == 3.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(550));
    assert(metric1.get() == 2.0);
    assert(metric2.get() == 3.0);
    assert(metric3.get() == 1.0);
}