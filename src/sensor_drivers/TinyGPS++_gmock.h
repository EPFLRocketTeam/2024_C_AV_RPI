# include <gmock/gmock.h>
#include "TinyGPS++_inteface.h"

class Mock_TinyGPSPlus : public TinyGPSPlus_Interface {
    public:
        MOCK_METHOD(bool, encode, (char c), (override));
        MOCK_METHOD(uint32_t, charsProcessed, (), (override));
        MOCK_METHOD(uint32_t, sentencesWithFix, (), (override));
        MOCK_METHOD(uint32_t, failedChecksum, (), (override));
};