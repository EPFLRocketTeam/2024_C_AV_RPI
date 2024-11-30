//
// Created by marin on 13.08.2024.
//

#ifndef FLIGHT_COMPUTER_MOCKSENSOR_H
#define FLIGHT_COMPUTER_MOCKSENSOR_H


#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "adxl375_gmock.h"
#include "bmi08_gmock.h"
#include "bmp3_gmock.h"
#include "I2CGPS_gmock.h"
#include "TinyGPS++_gmock.h"
#include "sensors.h"

class MockSensors  {
public:
    MockSensors();
    ~MockSensors() ;
    void calibrate() ;
    bool update() ;
    MockAdxl375 adxl1, adxl2;
    MockBMI08 bmi1, bmi2;
    MockBMP3 bmp1, bmp2;
    MockI2CGPS i2cgps;
    MockTinyGPSPlus gps;
    void update_status();
};
#endif //FLIGHT_COMPUTER_MOCKSENSOR_H

