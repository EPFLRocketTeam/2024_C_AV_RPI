#include <stdint.h>
#include "TinyGPS++.h"
class TinyGPSPlus_Interface{
    public:
        virtual ~TinyGPSPlus_Interface() = default;
        virtual bool encode(char c) = 0;
        virtual uint32_t charsProcessed() = 0;   
        virtual uint32_t sentencesWithFix() = 0; 
        virtual uint32_t failedChecksum() = 0;   
        virtual uint32_t passedChecksum() = 0;   
};