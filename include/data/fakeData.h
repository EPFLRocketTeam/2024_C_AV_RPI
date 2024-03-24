
#ifndef FAKEDATA_H
#define FAKEDATA_H

#include "fakeSensor.h"

class FakeData
{
public:
    FakeData();
    ~FakeData();

    void set_clean_data(char *data);

    void reset_cmd();
    SensFiltered dump() const;

    bool update();

private:
    SensFiltered clean_data;

    char *cmd;
};

#endif /* FAKEDATA_H */