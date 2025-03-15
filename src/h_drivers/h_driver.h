#ifndef H_DRIVER_H
#define H_DRIVER_H

#include "av_state.h"
#include "intranet_commands.h"

class HDriver {
public:
    virtual ~HDriver() = default;
    virtual void check_policy(const DataDump& dump) = 0;
};


#endif /* H_DRIVER_H */
