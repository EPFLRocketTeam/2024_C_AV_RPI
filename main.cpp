#include <iostream>
#include "dynconf.h"
#include <vector>
#include "module.h"
#include "adxl375_module.h"
#include "bmi088_module.h"
#include "bmp390_module.h"
#include "gps_module.h"
#include "ina228_module.h"
#include "tmp1075_module.h"
#include "sensors.h"
#include "logger.h"
#include "av_state.h"
#include <thread>
#include <chrono>

using namespace std;

int main(void){
    DataLogger &instance = DataLogger::getInstance();

    auto conv = [&instance]() -> void {
        DataDump dump = Data::get_instance().get();

        instance.conv(dump);
        return ;
    };
    uint64_t start_time = 0;
    auto time = [&start_time]() -> uint64_t {
        auto now    = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        return now_ms.time_since_epoch().count() - start_time;
    };
    start_time = time();
    auto useTimestamp = [&time]() -> void {
        Data& data = Data::get_instance();

        uint32_t timestamp(time());
        data.write(Data::GoatReg::AV_TIMESTAMP, &timestamp);
    };

    useTimestamp();
    conv();

    cout << "=== FC TEST ===" << endl;

    cout << "Load config..." << endl;
    ConfigManager::initConfig("./config.conf");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    useTimestamp();
    conv();
    cout << "=== Init Sensors HDriver ===" << endl;
    cout << endl;

    Sensors* driver = new Sensors();
    driver->init_sensors();
    
    useTimestamp();
    conv();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    State state = State::LIFTOFF;
    Data::get_instance().write(Data::GoatReg::AV_STATE, &state);

    useTimestamp();
    conv();

    AvState fsm;

    const uint32_t freq = 10;

    while (1) {
        uint32_t start = time();

        useTimestamp();

        DataDump state_dump = Data::get_instance().get();        
        fsm.update(state_dump);

        DataDump sensors_dump = Data::get_instance().get();     
        driver->check_policy(sensors_dump, sensors_dump.av_timestamp);
        conv();

        uint32_t end = time();

        if (start + freq > end) {
            uint32_t delta = start + freq - end;

            std::this_thread::sleep_for(std::chrono::milliseconds(delta));
        }
    }

    return 0;
}