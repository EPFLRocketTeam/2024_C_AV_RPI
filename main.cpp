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

    vector<string> module_targets = {
        "sensors.adxl1",
        "sensors.adxl2",
        "sensors.bmi1",
        "sensors.bmi2",
        "sensors.bmp1",
        "sensors.bmp2",
        "sensors.gps",
        "sensors.ina228_lpb",
        "sensors.ina228_hpb",
        "sensors.tmp1075"
    };

    for (string &target : module_targets) {
        bool c1 = ConfigManager::isEnabled(target, false);
        bool c2 = ConfigManager::isEnabled(target, true);
        
        if (c1 == c2) {
            cout << " - " << target << ": " << c1 << endl;
        } else {
            cout << " - " << target << ": missing entry in config." << endl;
        }
    }

    vector<SensorModule*> sensors;

    sensors.push_back( Adxl375Module::make_primary() );
    sensors.push_back( Adxl375Module::make_secondary() );

    sensors.push_back( Bmi088Module::make_primary() );
    sensors.push_back( Bmi088Module::make_secondary() );

    sensors.push_back( Bmp390Module::make_primary() );
    sensors.push_back( Bmp390Module::make_secondary() );

    sensors.push_back( INA228Module::make_lpb() );
    sensors.push_back( INA228Module::make_hpb() );

    sensors.push_back( GPSModule::make_gps() );

    sensors.push_back( Tmp1075Module::make_tmp() );
    
    useTimestamp();
    conv();

    for (SensorModule* module : sensors) {
        cout << endl;
        if (module == NULL) {
            cout << "NULL module" << endl;
            continue ;
        }

        cout << "Module " << module->get_name() << endl;
        cout << " - enabled " << module->is_enabled() << endl;
        cout << "Init module..." << endl;
        module->init();
        cout << " - enabled " << module->is_enabled() << endl;
        cout << " - failure " << module->is_failure() << endl;
    }

    cout << endl;
    cout << endl;
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

    for (size_t i = 0; i < 10; i ++) {
        useTimestamp();
        const DataDump dump = Data::get_instance().get();
        sensors->check_policy(dump, dump.av_timestamp);
        conv();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}