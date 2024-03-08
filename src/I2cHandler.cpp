// I2CHandler.cpp

#include "I2CHandler.h"

#include <iostream>
#include <pigpio.h>


I2CHandler::I2CHandler(int deviceAddress, int usedPins){
    // we use the pigpio, the pins it uses are 2 and 3 for i2c communication
    this->usedPins =   usedPins;
    this->deviceAddress = deviceAddress;
}

I2CHandler::~I2CHandler(){
    // Close the I2C handle using pigpio library
    //TODO
    this->deviceAddress = -1;
    return true;
}
//writing byte to the i2c device using pigpio library
bool I2CHandler::writeByte(uint8_t reg, PropCommands command) {
    data = propCommandsToBytes(command);
    // Open the I2C device
    int i2cHandle = i2cOpen(1, deviceAddress, 0);
    //PI_BAD_I2C_BUS, PI_BAD_I2C_ADDR,PI_BAD_FLAGS, PI_NO_HANDLE, or PI_I2C_OPEN_FAILED.
    errorHandling(i2cHandle);
    // Write a byte to data register
    int result = i2cWriteByteData(i2cHandle, 1<<Regs::Data, data);
    if (result == -1) {
        std::cerr << "Failed to write byte to I2C device." << std::endl;
        i2cClose(i2cHandle);
        return false;
    }else{
        // in console print what we sent to the i2c device
        std::cout << "Sent: " << commandToString(data) << std::endl;
        i2CClose(i2cHandle);
        return true;
    }
}


bool I2CHandler::readByte(uint8_t reg, char* data,Regs reg) {
    // Open the I2C device
    int i2cHandle = i2cOpen(1, deviceAddress, 0);
    //PI_BAD_I2C_BUS, PI_BAD_I2C_ADDR,PI_BAD_FLAGS, PI_NO_HANDLE, or PI_I2C_OPEN_FAILED.
    errorHandling(i2cHandle);
    // Read a byte from data register
    int result = i2cReadByteData(i2cHandle, 1<<reg);
    if (result == -1) {
        std::cerr << "Failed to read byte from I2C device." << std::endl;
        i2cClose(i2cHandle);
        return false;
    }else{
        // in console print what we received from the i2c device
        *data = result;
        std::cout << "Received: " << *data << std::endl;
        i2CClose(i2cHandle);
        return true;
    }




    
}

char* commandToString(PropCommands command){
    switch (command)
    {
    case PropCommands::IGNITE:
        return "IGNITE";
        break;
    case PropCommands::ARM:
        return "ARM";
        break;
    case PropCommands::DISARM:
        return "DISARM";
        break;
    case PropCommands::CALIBRATE:
        return "CALIBRATE";
        break;
    case PropCommands::DESCENT:
        return "DESCENT";
        break;
    case PropCommands::ASCENT:
        return "ASCENT";
        break;
    case PropCommands::MANUAL:
        return "MANUAL";
        break;
    case PropCommands::THRUSTSEQUENCE:
        return "THRUSTSEQUENCE";
        break;
    case PropCommands::READY:
        return "READY";
        break;
    case PropCommands::ERRORGROUND:
        return "ERRORGROUND";
        break;
    case PropCommands::ERRORFLIGHT:
        return "ERRORFLIGHT";
        break;
    case PropCommands::LANDED:
        return "LANDED";
        break;
    case PropCommands::IDLE:
        return "IDLE";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

char propCommandsToBytes(PropCommands command){
    switch (command)
    {
    case PropCommands::IGNITE:
        return 0x01;
        break;
    case PropCommands::ARM:
        return 0x02;
        break;
    case PropCommands::DISARM:
        return 0x03;
        break;
    case PropCommands::CALIBRATE:
        return 0x04;
        break;
    case PropCommands::DESCENT:
        return 0x05;
        break;
    case PropCommands::ASCENT:
        return 0x06;
        break;
    case PropCommands::MANUAL:
        return 0x07;
        break;
    case PropCommands::THRUSTSEQUENCE:
        return 0x08;
        break;
    case PropCommands::READY:
        return 0x09;
        break;
    case PropCommands::ERRORGROUND:
        return 0x0A;
        break;
    case PropCommands::ERRORFLIGHT:
        return 0x0B;
        break;
    case PropCommands::LANDED:
        return 0x0C;
        break;
    case PropCommands::IDLE:
        return 0x0D;
        break;
    default:
        return 0x00;
        break;
    }
}

void errorHandling(int res){
    switch (res)
    {
    case PI_BAD_I2C_BUS:
        std::cerr << "Bad I2C bus." << std::endl;
        break;
    
    case PI_BAD_I2C_ADDR:
        std::cerr << "Bad I2C address." << std::endl;
        break;
    case PI_BAD_FLAGS:
        std::cerr << "Bad I2C flags." << std::endl;
        break;
    case PI_NO_HANDLE:
        std::cerr << "No handle." << std::endl;
        break;
    case PI_I2C_OPEN_FAILED:
        std::cerr << "I2C open failed." << std::endl;
        break;

    default:
        break;
    }
}
