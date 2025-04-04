#include <iostream>
#include <unistd.h>
#include <cassert>
#include "TMP1075.h"
#include <fstream>

int main()
{

    std::ofstream log("/boot/av_log/tmp1075_test.log", std::ios::app);
    log << "init test" << std::endl;

    try
    {
        TMP1075 sensor(TMP1075_ADDR_I2C);
        log << "TMP1075 sensor initialized successfully." << std::endl;

        uint16_t deviceId = sensor.getDeviceId();
        log << "Device ID: 0x" << std::hex << deviceId << std::dec << std::endl;

        for (int i = 0; i < 5; i++)
        {
            int16_t rawTemp = sensor.getTemperatureRaw();
            float tempC = sensor.getTemperatureCelsius();


            
            log << "Iteration " << i << ":" << std::endl;
            log << "  Raw Temperature: " << rawTemp << std::endl;
            log << "  Temperature: " << tempC << " °C" << std::endl;

            assert(tempC >= -40.0f && tempC <= 125.0f);

            usleep(1000000);
        }

        log << "Test completed successfully." << std::endl;
    }
    catch (TMP1075Exception &e)
    {
        log << e.what() << std::endl;
        log.close();
        return -1;
    }
log.close();
    return 0;
}
