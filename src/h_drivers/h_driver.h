#ifndef H_DRIVER_H
#define H_DRIVER_H

#include "data.h"
#include "av_state.h"
#include "2024_C_AV_INTRANET/intranet_commands.h"

class HDriver {
public:
    HDriver();
    virtual ~HDriver() = default;

    virtual void check_policy(Data::GoatReg reg, DataDump dump);    
};

#endif /* H_DRIVER_H */