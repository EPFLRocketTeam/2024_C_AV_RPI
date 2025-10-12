#ifndef H_DRIVER_H
#define H_DRIVER_H

#include <cstdint>

struct DataDump;

bool IS_PRES_VALID(const float pressure);
bool IS_TEMP_VALID(const float temperature);

class HDriver {
public:
    virtual ~HDriver() = default;
    /**
     *  @param dump:        Goat struct
     *  @param delta_ms:    Delta time (time difference between last call and current call, in ms) 
     */
    virtual void check_policy(const DataDump& dump, const uint32_t delta_ms) = 0;
};


#endif /* H_DRIVER_H */
