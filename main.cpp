#include <iostream>
#include <unistd.h>
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
#include "buzzer.h"
#include <thread>
#include <chrono>

#include "pigpio.h"
#include "config.h"
#include "i2c_interface.h"
#include <algorithm>

int main(void){
    DataLogger &instance = DataLogger::getInstance();

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

    ConfigManager::initConfig("./config.conf");
    usleep(100e3);

    useTimestamp();

    Sensors driver;
    driver.init_sensors();
    
    useTimestamp();
    usleep(100e3);

    State state = State::LIFTOFF;
    Data::get_instance().write(Data::GoatReg::AV_STATE, &state);

    useTimestamp();

    AvState fsm;

    const uint32_t freq = 10;

    bool done_buzzer = false;
    //uint32_t end_target = 0;

    while (1) {
        uint32_t start = time();
        if (start >= 10000 && !done_buzzer) {
            done_buzzer = true;
            std::map<std::string, bool> _mp = driver.sensors_status();
            std::cout << "Making buzzer payload\n";

            for (auto u : _mp) {
                std::cout << u.first << ": " << u.second << "\n";
                Buzzer::enable();
                usleep(250e3);
                Buzzer::disable();
                usleep(750e3);

                if (u.second) {
                    for (int i(0); i < 5; ++i) {
                        Buzzer::toggle();
                        usleep(100e3);
                        Buzzer::toggle();
                        usleep(100e3);
                    }
                }
            }
        //    std::reverse(payload.begin(), payload.end());
        }

        /*
        if (start >= end_target) {
            if (payload.size() == 0) {
                gpioWrite(BUZZER, 0);
            } else {
                BuzzerTarget ntar = payload.back();

                end_target = start + ntar.duration;
                ntar.enable();

                payload.pop_back();
            }
        }
        */

        useTimestamp();

        DataDump state_dump = Data::get_instance().get();        
        fsm.update(state_dump);

        DataDump sensors_dump = Data::get_instance().get();     
        driver.check_policy(sensors_dump, sensors_dump.av_timestamp);

        uint32_t end = time();
        if (start + freq > end) {
            uint32_t delta = start + freq - end;
            std::this_thread::sleep_for(std::chrono::milliseconds(delta));
        }
    }

    return 0;
}
