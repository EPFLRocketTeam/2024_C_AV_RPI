#include <iostream>
#include <cassert>
#include <unistd.h>
#include "INA228.h"
#include <fstream>

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION 3

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/
int main() {
    std::fstream log("/boot/av_log/ina228_test.log", std::ios::app);
    log << "before try"<< std::endl;
    try
    {
        log << " start test" << std::endl;

        INA228 ina(INA228_ADDRESS_HPB);
    
        if (!ina.isConnected()){
            std::cerr << "INA228 not connected." << std::endl;
            return -1;
        }
        
        log << "INA228 test starting..." << std::endl;
        
        ina.reset();
        
        for (int loop = 0; loop < ITERATION; loop++){
            float busVoltage    = ina.getBusVoltage();
            float shuntVoltage  = ina.getShuntVoltage();
            float current       = ina.getCurrent();
            float power         = ina.getPower();
            float temperature   = ina.getTemperature();
            double energy       = ina.getEnergy();
            double charge       = ina.getCharge();
            
            log << "Iteration " << loop << ":\n";
            log << "  Bus Voltage: " << busVoltage << " V\n";
            log << "  Shunt Voltage: " << shuntVoltage << " V\n";
            log << "  Current: " << current << " A\n";
            log << "  Power: " << power << " W\n";
            log << "  Temperature: " << temperature << " °C\n";
            log << "  Energy: " << energy << " J\n";
            log << "  Charge: " << charge << " C\n";
        
            assert(busVoltage >= 0 && busVoltage <= 100);
            assert(shuntVoltage > -0.1 && shuntVoltage < 0.1);
            assert(temperature >= -40 && temperature <= 125);
            
            sleep(1);  
        }
        
        log << "INA228 test completed." << std::endl;
    }
    catch(INA228Exception& e)
    {
        log << e.what() << '\n';
        log.close();
        return -1;
    }
    log.close();
    
   
    return 0;
}
