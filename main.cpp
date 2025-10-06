#include <iostream>
#include <unistd.h>
#include <pigpio.h>
#include <vector>
#include <cmath>
#include "dynconf.h"
#include "module.h"
#include "sensors.h"
#include "logger.h"
#include "av_state.h"
#include "buzzer.h"
#include "av_timer.h"
#include "config.h"
#include "trigger_board.h"
#include "PR_board.h"
#include "dpr.h"
#include "telecom.h"
#include "intranet_commands.h"

#define PRB_ERROR_LIMIT 100

#define SLEEP_MS_DEFAULT 100
#define SLEEP_MS_VALID (SLEEP_MS_DEFAULT>>1)
#define SLEEP_MS_NEXT_MODULE (SLEEP_MS_DEFAULT*10)
#define MS_TO_S 1000

int main() {
    if (!Logger::init()) {
        std::cout << "Failed opening log files.\n";
        return 1;
    }

    ConfigManager::initConfig("./config.conf");
    AvTimer::sleep(100);

    AvState fsm;
    State fsm_state(fsm.getCurrentState());
    Data::get_instance().write(Data::GoatReg::AV_STATE, &fsm_state);

    Sensors sensors;
    sensors.init_sensors();
    
    AvTimer::sleep(SLEEP_MS);

    // HDrivers
    TriggerBoard trigger_board;
    PR_board prop_board;
    DPR dpr_ethanol(AV_NET_ADDR_DPR_ETH);
    DPR dpr_lox(AV_NET_ADDR_DPR_LOX);

    AvTimer::sleep(SLEEP_MS_DEFAULT);

    // Telecom
    Telecom telecom;
    try {
        telecom.begin();
    }catch(TelecomException& e) {
        Logger::log_eventf(Logger::ERROR, "%s", e.what());
    }

    std::map<std::string, bool> _mp = sensors.sensors_status();
    Logger::log_eventf("Verifying sensors status...");
    for (auto u : _mp) {
        Logger::log_eventf("%s: %b", u.first.c_str(), u.second);
        
        Buzzer::enable();
        AvTimer::sleep(SLEEP_MS_DEFAULT);
        Buzzer::disable();
        AvTimer::sleep(SLEEP_MS_DEFAULT*5);
        
        if (u.second) {
            for (int i(0); i < 3; ++i) {
                
                Buzzer::toggle();
                AvTimer::sleep(SLEEP_MS_VALID);
                Buzzer::toggle();
                AvTimer::sleep(SLEEP_MS_VALID);
                
                }
        }
        AvTimer::sleep(SLEEP_MS_NEXT_MODULE);
    }

    const uint32_t inv_freq = MS_TO_S * (float)(1.0 / MAIN_LOOP_MAX_FREQUENCY);
    uint32_t now_ms(AvTimer::tick());
    uint32_t old_ms(0);
    uint32_t delta_ms(0);
    uint32_t prop_cons_error_count(0);
    while (1) {
        old_ms = now_ms;
        now_ms = AvTimer::tick();
        delta_ms = now_ms - old_ms;

        // Write timestamp and retreive GOAT object 
        Data::get_instance().write(Data::AV_TIMESTAMP, &now_ms);
        Data::get_instance().write(Data::AV_DELTA_MS, &delta_ms);
        DataDump dump = Data::get_instance().get();        
        // Data logging
        Logger::log_dump(dump);
        // FSM
        fsm.update(dump,delta_ms);


        // Retrieve sensors data
        sensors.check_policy(dump, delta_ms);

        // Execute PRB
        try {
            prop_board.check_policy(dump, delta_ms);
            // Reset error counter on success
            prop_cons_error_count = 0;
        }
        catch (PRBoardException &e)
        {
            prop_cons_error_count++;
            if (prop_cons_error_count >= PRB_ERROR_LIMIT)
            {
                Logger::log_eventf(Logger::ERROR, "%s", e.what());
            }
        }
        // Execute DPRs
        try {
            dpr_ethanol.check_policy(dump, delta_ms);
        }catch(DPRException& e) {
            Logger::log_eventf(Logger::ERROR, "%s", e.what());
        }
        
        try {
            dpr_lox.check_policy(dump, delta_ms);
        }catch(DPRException& e) {
            Logger::log_eventf(Logger::ERROR, "%s", e.what());
        }

        const float copv_p_eth(dump.prop.N2_pressure_eth);
        const float copv_p_lox(dump.prop.N2_pressure_lox);
        float copv_pressure((copv_p_eth + copv_p_lox) * 0.5);
        Data::get_instance().write(Data::PR_SENSOR_P_NCO, &copv_pressure);

        // Execute TRB
        
        try {
            trigger_board.check_policy(dump, delta_ms);
        }catch(TriggerBoardException& e) {
            Logger::log_eventf(Logger::ERROR, "%s", e.what());
        }
        


        // Execute telemetry
        try {
            telecom.check_policy(dump, delta_ms);
        }catch(TelecomException& e) {
            Logger::log_eventf(Logger::ERROR, "%s", e.what());
        }
        

        // If loop finished early, compensate
        if (delta_ms < inv_freq) {
            AvTimer::sleep(inv_freq - delta_ms);
        }

        if (dump.av_state == State::INIT) {
            Logger::log_eventf("Delta_ms: %u", delta_ms);
        }
    }

    Logger::terminate();
    return 0;
}
