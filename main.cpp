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
    AvTimer::sleep(SLEEP_MS_DEFAULT);

    AvState fsm;
    State fsm_state(fsm.getCurrentState());
    Data::get_instance().write(Data::GoatReg::AV_STATE, &fsm_state);

    Sensors sensors;
    sensors.init_sensors();
    
    AvTimer::sleep(SLEEP_MS_DEFAULT);

    //
    // HDrivers initialization and status test
    Logger::log_eventf("Initializing HDrivers and performing I2C status test...");
    bool trigger_board_status(1);
    TriggerBoard trigger_board;
    try {
        trigger_board.write_timestamp();
        Logger::log_eventf("TRB OK");
    }catch(TriggerBoardException& e) {
        Logger::log_eventf(Logger::FATAL, "TRB not responding");
        trigger_board_status = 0;
    }

    bool pr_board_status(1);
    PR_board prop_board;
    try {
        prop_board.write_timestamp();
        Logger::log_eventf("PRB OK");
    }catch(PRBoardException& e) {
        Logger::log_eventf(Logger::FATAL, "PRB not responding");
        pr_board_status = 0;
    }

    bool dpr_ethanol_status(1);
    DPR dpr_ethanol(AV_NET_ADDR_DPR_ETH);
    try {
        dpr_ethanol.write_timestamp();
        Logger::log_eventf("DPR_ETH OK");
    }catch(DPRException& e) {
        Logger::log_eventf(Logger::FATAL, "DPR Ethanol not responding");
        dpr_ethanol_status = 0;
    }

    bool dpr_lox_status(1);
    DPR dpr_lox(AV_NET_ADDR_DPR_LOX);
    try {
        dpr_lox.write_timestamp();
        Logger::log_eventf("DPR_LOX OK");
    }catch(DPRException& e) {
        Logger::log_eventf(Logger::FATAL, "DPR LOX not responding");
        dpr_lox_status = 0;
    }

    AvTimer::sleep(SLEEP_MS_DEFAULT);

    // Telecom
    Telecom telecom;
    bool telecom_status(0);
    try {
        telecom_status = telecom.begin();
        Logger::log_eventf("Telecom OK");
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

    Buzzer::enable();
    AvTimer::sleep(SLEEP_MS_DEFAULT);
    Buzzer::disable();
    AvTimer::sleep(SLEEP_MS_DEFAULT*5);
    if (telecom_status) {
        for (int i(0); i < 3; ++i) {
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);

        }
    }
    AvTimer::sleep(SLEEP_MS_NEXT_MODULE);

    Buzzer::enable();
    AvTimer::sleep(SLEEP_MS_DEFAULT);
    Buzzer::disable();
    AvTimer::sleep(SLEEP_MS_DEFAULT*5);
    if (trigger_board_status) {
        for (int i(0); i < 3; ++i) {
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);

        }
    }
    AvTimer::sleep(SLEEP_MS_NEXT_MODULE);

    Buzzer::enable();
    AvTimer::sleep(SLEEP_MS_DEFAULT);
    Buzzer::disable();
    AvTimer::sleep(SLEEP_MS_DEFAULT*5);
    if (pr_board_status) {
        for (int i(0); i < 3; ++i) {
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);

        }
    }
    AvTimer::sleep(SLEEP_MS_NEXT_MODULE);

    Buzzer::enable();
    AvTimer::sleep(SLEEP_MS_DEFAULT);
    Buzzer::disable();
    AvTimer::sleep(SLEEP_MS_DEFAULT*5);
    if (dpr_ethanol_status) {
        for (int i(0); i < 3; ++i) {
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);

        }
    }
    AvTimer::sleep(SLEEP_MS_NEXT_MODULE);

    Buzzer::enable();
    AvTimer::sleep(SLEEP_MS_DEFAULT);
    Buzzer::disable();
    AvTimer::sleep(SLEEP_MS_DEFAULT*5);
    if (dpr_lox_status) {
        for (int i(0); i < 3; ++i) {
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);
            Buzzer::toggle();
            AvTimer::sleep(SLEEP_MS_VALID);

        }
    }
    AvTimer::sleep(SLEEP_MS_NEXT_MODULE);
    // Modules and HDrivers status buzzer END
    // --------------------------


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
            
            if (prop_cons_error_count >= PRB_ERROR_LIMIT)
            {
                Logger::log_eventf(Logger::ERROR, "%s", e.what());
            }
            else
            {
                Logger::log_eventf(Logger::DEBUG, "PRB communication error %d/%d", prop_cons_error_count, PRB_ERROR_LIMIT);
                prop_cons_error_count++;
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
