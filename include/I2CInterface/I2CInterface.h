#ifndef I2CINTERFACE_H
#define I2CINTERFACE_H

#include <cstdint>
#include <stdexcept>
#include <string>

class I2CInterfaceException : public std::runtime_error {
public:
    explicit I2CInterfaceException(const std::string& msg) : std::runtime_error(msg) {}
};

// Singleton class because there is only one i2c interface.
class I2CInterface {
private:
    I2CInterface() { initialize(); } // Constructor is private now
    ~I2CInterface() { terminate(); }
    void initialize();
    void terminate();
    uint8_t handle_list[0x7F] = {0};
    uint8_t i2c_addr_list[0x7F] = {0};
public:
    // getInstance initializes the singleton class on first call, otherwise it returns
    // that instance. Makes the initialisation automatic for any i2c function call
    static I2CInterface& getInstance() {
        static I2CInterface instance;
        return instance;
    }
    void open(uint8_t addr);
    void close(uint8_t addr);
    void write(uint8_t addr, uint8_t reg_addr, const uint8_t* data, uint32_t len);
    void read(uint8_t addr, uint8_t reg_addr, uint8_t* data, uint32_t len);
    void* get_intf_ptr(uint8_t addr);
    I2CInterface(I2CInterface const&) = delete; // Prevent copying
    void operator=(I2CInterface const&) = delete; // Prevent assignment
};

#endif // I2CINTERFACE_H

