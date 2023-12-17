#include <iostream>
#include <wiringPiI2C.h>

int main() {
    const int i2cBus = 1; // Use 1 for Raspberry Pi 3 and newer (0 for older versions)
    const int minAddress = 0x03;
    const int maxAddress = 0x77;

    std::cout << "Scanning I2C bus..." << std::endl;

    for (int address = minAddress; address <= maxAddress; ++address) {
        int device = wiringPiI2CSetup(address);
        if (device != -1) {
            std::cout << "I2C device found at address 0x" << std::hex << address << std::dec << std::endl;
            wiringPiI2CWrite(device, 0); // You can perform additional operations if needed
            wiringPiI2CClose(device);
        }
    }

    return 0;
}