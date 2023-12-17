// I2CHandler.h



#include <cstdint>

class I2CHandler {
    

public:
    I2CHandler(int deviceAddress);
    ~I2CHandler();

    // Write a byte to a specific register
    bool writeByte(uint8_t reg, uint8_t data);

    // Read a byte from a specific register
    bool readByte(uint8_t reg, uint8_t& data);
    char* commandToString(PropCommands command);
    void errorHandling(int res);
    int usedPins;

private:
// device pin for i2c communication of arduino nano are A6 and A7 for inputs
// device pin for i2c communication for teensy is A3/17 
    int deviceAddress;
    char propCommandsToBytes(PropCommands command);


};

enum class PropCommands{
    IGNITE,
    ARM,
    DISARM,
    CALIBRATE,
    DESCENT,
    ASCENT,
    MANUAL,
    THRUSTSEQUENCE,
    READY,
    ERRORGROUND,
    ERRORFLIGHT,
    LANDED,
    IDLE
};

enum class Regs{
   Control,
    Status,
    Data,
    Clock,
    Adresse
};
