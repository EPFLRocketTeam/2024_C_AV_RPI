# include <gmock/gmock.h>
#include "TinyGPS++_interface.h"
#include "TinyGPS++.h"
class MockTinyGPSPlus : public TinyGPSPlus_Interface {
    public:
        MOCK_METHOD(bool, encode, (char c), (override));
        MOCK_METHOD(uint32_t, charsProcessed, (), (const, override));
        MOCK_METHOD(uint32_t, sentencesWithFix, (), (const, override));
        MOCK_METHOD(uint32_t, failedChecksum, (), (const, override));
        MOCK_METHOD(uint32_t, passedChecksum, (), (const, override));
        TinyGPSLocation location;
        TinyGPSDate date;
        TinyGPSTime time;
        TinyGPSSpeed speed;
        TinyGPSCourse course;
        TinyGPSAltitude altitude;
        TinyGPSInteger satellites;
        TinyGPSHDOP hdop;
        const TinyGPSLocation &getLocation() const override { return location; }
        const TinyGPSDate &getDate() const override { return date; }
        const TinyGPSTime &getTime() const override { return time; }
        const TinyGPSSpeed &getSpeed() const override { return speed; }
        const TinyGPSCourse &getCourse() const override { return course; }
        const TinyGPSAltitude &getAltitude() const override { return altitude; }
        const TinyGPSInteger &getSatellites() const override { return satellites; }
        const TinyGPSHDOP &getHDOP() const override { return hdop; }
        
};