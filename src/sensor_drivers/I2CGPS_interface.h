#ifndef I2CGPS_INTERFACE_H
#define I2CGPS_INTERFACE_H
#include <stdint.h>
#include <string>


class I2CGPS_Interface {
    public:
    virtual ~I2CGPS_Interface() = default;
    virtual void check() = 0;
    virtual uint8_t available() = 0;
    virtual uint8_t read() = 0;
    virtual void sendMTKpacket(std::string command) = 0;
    virtual std::string createMTKpacket(uint16_t packetType, std::string dataField) = 0;
    virtual std::string calcCRCforMTK(std::string sentence) = 0 ;
    virtual void sendPGCMDpacket(std::string command) = 0;
    virtual std::string createPGCMDpacket(uint16_t packetType, std::string dataField) = 0;
};
#endif //I2CGPS_INTERFACE_H