#include <iostream>
#include <cassert>
#include <unistd.h>
#include "INA228.h"

/************************************************************************/
/*********                     Macros                              ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION 3

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/
int main() {
    try
    {
        INA228 ina(0x40);
    
        if (!ina.isConnected()){
            std::cerr << "INA228 not connected." << std::endl;
            return -1;
        }
        
        std::cout << "INA228 test starting..." << std::endl;
        
        ina.reset();
        
        for (int loop = 0; loop < ITERATION; loop++){
            float busVoltage    = ina.getBusVoltage();
            float shuntVoltage  = ina.getShuntVoltage();
            float current       = ina.getCurrent();
            float power         = ina.getPower();
            float temperature   = ina.getTemperature();
            double energy       = ina.getEnergy();
            double charge       = ina.getCharge();
            
            std::cout << "Iteration " << loop << ":\n";
            std::cout << "  Bus Voltage: " << busVoltage << " V\n";
            std::cout << "  Shunt Voltage: " << shuntVoltage << " V\n";
            std::cout << "  Current: " << current << " A\n";
            std::cout << "  Power: " << power << " W\n";
            std::cout << "  Temperature: " << temperature << " °C\n";
            std::cout << "  Energy: " << energy << " J\n";
            std::cout << "  Charge: " << charge << " C\n";
        
            assert(busVoltage >= 0 && busVoltage <= 100);
            assert(shuntVoltage > -0.1 && shuntVoltage < 0.1);
            assert(temperature >= -40 && temperature <= 125);
            
            sleep(1);  
        }
        
        std::cout << "INA228 test completed." << std::endl;
    }
    catch(INA228Exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    
   
    return 0;
}
