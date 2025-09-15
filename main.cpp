#include <iostream>
#include <unistd.h>
#include <pigpio.h>
#include <vector>
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
    
    AvTimer::sleep(100);

    // HDrivers
    //TriggerBoard trigger_board;
    PR_board prop_board;
    DPR dpr_ethanol(AV_NET_ADDR_DPR_ETH);
    DPR dpr_lox(AV_NET_ADDR_DPR_LOX);

    AvTimer::sleep(100);

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
        /*
        Buzzer::enable();
        AvTimer::sleep(250);
        Buzzer::disable();
        AvTimer::sleep(750);
        */
        if (u.second) {
            for (int i(0); i < 5; ++i) {
                /*
                Buzzer::toggle();
                AvTimer::sleep(100);
                Buzzer::toggle();
                AvTimer::sleep(100);
                */
                }
        }
    }

    const uint32_t inv_freq = 1000 * (float)(1.0 / MAIN_LOOP_MAX_FREQUENCY);
    uint32_t now_ms(AvTimer::tick());
    uint32_t old_ms(0);
    uint32_t delta_ms(0);
    while (1) {
        old_ms = now_ms;
        now_ms = AvTimer::tick();
        delta_ms = now_ms - old_ms;

        // Write timestamp and retreive GOAT object 
        Data::get_instance().write(Data::AV_TIMESTAMP, &now_ms);
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
        }catch(PRBoardException& e) {
            Logger::log_eventf(Logger::ERROR, "%s", e.what());
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

        // -----------
        // TODO: MOVE ELSEWHERE
        // COPV Readings averaging both DPRs
        float copv_pressure(0);
        float copv_press_eth(0);
        bool copv_press_eth_ok(1);
        float copv_press_lox(0);
        bool copv_press_lox_ok(1);
        try {
            copv_press_eth = dpr_ethanol.read_copv_pressure();
        }catch(DPRException& e) {
            Logger::log_eventf(Logger::ERROR, "%s", e.what());
            copv_press_eth_ok = false;
        }
        try {
            copv_press_lox = dpr_lox.read_copv_pressure();
        }catch(DPRException& e) {
            Logger::log_eventf(Logger::DEBUG, "%s", e.what());
            copv_press_lox_ok = false;
        }
        // TODO: Filter sensors with lower and higher bounds.
        // Rationale: do not allow spurious readings to interfere with FSM and be sent to GS
        // DO THIS ON EVERY PR SENSOR ? IN A NEW FILE ?
        if (copv_press_eth > 500) {
            copv_press_eth_ok = 1;
        }
        if (copv_press_lox > 500) {
            copv_press_lox_ok = 1;
        }
        if (copv_press_eth_ok || copv_press_lox_ok) {
            copv_pressure = (copv_press_eth + copv_press_lox) / (copv_press_eth_ok + copv_press_lox_ok);
            Data::get_instance().write(Data::PR_SENSOR_P_NCO, &copv_pressure);
        }
        // ---------

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
    }

    Logger::terminate();
    return 0;
}
