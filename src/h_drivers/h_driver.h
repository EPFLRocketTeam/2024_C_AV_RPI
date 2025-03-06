#ifndef H_DRIVER_H
#define H_DRIVER_H

#include "data.h"
#include "av_state.h"

class HDriver {
    public:
        HDriver() = default;
        virtual ~HDriver() = default;
    
        virtual void check_policy(Data::GoatReg reg, const DataDump& dump) = 0; // Pure virtual
};
    

#endif /* H_DRIVER_H */