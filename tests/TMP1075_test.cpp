#include <iostream>
#include <unistd.h>
#include <cassert>
#include "TMP1075.h"

int main()
{

    try
    {
        TMP1075 sensor(TMP1075_ADDR_I2C);
        std::cout << "TMP1075 sensor initialized successfully." << std::endl;

        uint16_t deviceId = sensor.getDeviceId();
        std::cout << "Device ID: 0x" << std::hex << deviceId << std::dec << std::endl;

        for (int i = 0; i < 5; i++)
        {
            int16_t rawTemp = sensor.getTemperatureRaw();
            float tempC = sensor.getTemperatureCelsius();

            std::cout << "Iteration " << i << ":" << std::endl;
            std::cout << "  Raw Temperature: " << rawTemp << std::endl;
            std::cout << "  Temperature: " << tempC << " °C" << std::endl;

            assert(tempC >= -40.0f && tempC <= 125.0f);

            usleep(1000000);
        }

        std::cout << "Test completed successfully." << std::endl;
    }
    catch (TMP1075Exception &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
