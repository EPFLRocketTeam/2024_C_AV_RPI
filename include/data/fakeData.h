
#ifndef FAKEDATA_H
#define FAKEDATA_H


#include "fakeSensor.h"


class FakeData{
public:
    FakeData();
    ~FakeData();

    void set_raw_data(SensRaw data);
    void set_clean_data(SensFiltered data);
    void set_status(SensStatus status);
    
    void reset_cmd();
    SensStatus get_status() const;
    SensFiltered dump() const;

    bool update();
    
    private:
    SensRaw raw_data;
    SensFiltered clean_data;
    SensStatus status;
    char* cmd;
    };

#endif /* FAKEDATA_H */