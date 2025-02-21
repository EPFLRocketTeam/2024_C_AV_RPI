#ifndef H_DRIVER_H
#define H_DRIVER_H

#include "data.h"

class HDriver {
public:
    HDriver();
    ~HDriver() = default;

    static void check_policy(Data::GoatReg reg, DataDump dump);    
};

#endif /* H_DRIVER_H */