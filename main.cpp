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
#include "av_timer.h"

#include "pigpio.h"
#include "config.h"
#include "i2c_interface.h"
#include <algorithm>

int main(void){
    DataLogger &instance = DataLogger::getInstance();

    ConfigManager::initConfig("./config.conf");
    AvTimer::sleep(100);

    Sensors driver;
    driver.init_sensors();
    
    AvTimer::sleep(100);

    State state = State::LIFTOFF;
    Data::get_instance().write(Data::GoatReg::AV_STATE, &state);

    AvState fsm;

    const uint32_t freq = 10;

    bool done_buzzer = false;

    uint32_t now_ms(AvTimer::tick());
    uint32_t old_ms(0);
    uint32_t delta_ms(0);
    while (1) {
        old_ms = now_ms;
        now_ms = AvTimer::tick();
        delta_ms = now_ms - old_ms;

        Data::get_instance().write(Data::AV_TIMESTAMP, &now_ms);

        uint32_t start = AvTimer::tick();
        if (start >= 10000 && !done_buzzer) {
            done_buzzer = true;
            std::map<std::string, bool> _mp = driver.sensors_status();
            std::cout << "Making buzzer payload\n";

            for (auto u : _mp) {
                std::cout << u.first << ": " << u.second << "\n";
                Buzzer::enable();
                AvTimer::sleep(250);
                Buzzer::disable();
                AvTimer::sleep(750);

                if (u.second) {
                    for (int i(0); i < 5; ++i) {
                        Buzzer::toggle();
                        AvTimer::sleep(100);
                        Buzzer::toggle();
                        AvTimer::sleep(100);
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

        DataDump dump = Data::get_instance().get();        
        fsm.update(dump);

        driver.check_policy(dump, delta_ms);

        if (delta_ms < freq) {
            AvTimer::sleep(freq - delta_ms);
        }
    }

    return 0;
}
