#ifndef TELECOM_MOCK_H
#define TELECOM_MOCK_H

#include "Telecom_Interface.h"
#include <gmock/gmock.h>

class MockTelecom : public Telecom_Interface {
public:
    MOCK_METHOD(bool, begin, (), (override));
    MOCK_METHOD(void, send_telemetry, (), (override));
    MOCK_METHOD(void, update, (), (override));
    MOCK_METHOD(void, reset_cmd, (), (override));
    MOCK_METHOD(void, handle_capsule_uplink, (uint8_t packet_id, uint8_t* data_in, uint32_t len), (override));
    MOCK_METHOD(void, handle_capsule_downlink, (uint8_t packet_id, uint8_t* data_in, uint32_t len), (override));
};


#endif // TELECOM_MOCK_H
