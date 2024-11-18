#include <stdint.h>
#include "TinyGPS++.h"
class TinyGPSPlus_Interface{
    public:
        virtual ~TinyGPSPlus_Interface() = default;
        virtual bool encode(char c) = 0;
        virtual uint32_t charsProcessed() const = 0;   
        virtual uint32_t sentencesWithFix() const= 0; 
        virtual uint32_t failedChecksum() const= 0;   
        virtual uint32_t passedChecksum() const= 0;   
        
};