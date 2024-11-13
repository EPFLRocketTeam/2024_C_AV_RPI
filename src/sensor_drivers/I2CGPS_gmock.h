# include <gmock/gmock.h>
# include "I2CGPS_interface.h"
class MockI2CGPS : public I2CGPS_Interface {
public:
    MOCK_METHOD(void, check, (), (override));
    MOCK_METHOD(uint8_t, available, (), (override));
    MOCK_METHOD(uint8_t, read, (), (override));
    MOCK_METHOD(void, sendMTKpacket, (std::string command), (override));
    MOCK_METHOD(std::string, createMTKpacket, (uint16_t packetType, std::string dataField), (override));
    MOCK_METHOD(std::string, calcCRCforMTK, (std::string sentence), (override));
    MOCK_METHOD(void, sendPGCMDpacket, (std::string command), (override));
    MOCK_METHOD(std::string, createPGCMDpacket, (uint16_t packetType, std::string dataField), (override));

};