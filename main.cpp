#include <iostream>
#include "dynconf.h"
#include <vector>

using namespace std;

int main(void){
    cout << "=== FC TEST ===" << endl;

    cout << "Load config..." << endl;
    ConfigManager::initConfig("./config.conf");

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

    return 0;
}