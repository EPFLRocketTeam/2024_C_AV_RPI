//
// Created by marin on 13.08.2024.
//

#ifndef FLIGHT_COMPUTER_MOCKSENSOR_H
#define FLIGHT_COMPUTER_MOCKSENSOR_H

#endif //FLIGHT_COMPUTER_MOCKSENSOR_H

#include "gmock/gmock.h"
#include "../../data/include/sensors.h"

class MockSensor : public Sensors {
public:
    MOCK_METHOD(bool,update,(),(override) );
};
