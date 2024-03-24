
#include "../include/data/fakeData.h"
#include <string.h>
#include <stdlib.h>
#include "../include/data/fakeSensor.h"

FakeData::FakeData()
{
    clean_data = SensFiltered();
    cmd = NULL;
}

FakeData::~FakeData()
{
}

SensFiltered FakeData::dump() const
{
    return clean_data;
}

void FakeData::set_clean_data(char *data)
{
    // parse -> altitude(m):165.0;velocity(m/s):0.0;Time(ms):4446.0;acceleration: (0.1569064, 8.394492399999999, -5.962443199999999);pressure(Pa):1005.68780646843;command:0
    char *token = strtok(data, ";");
    while (token != NULL)
    {
        char *key = strtok(token, ":");
        char *value = strtok(NULL, ":");
        if (strcmp(key, "altitude(m)") == 0)
        {
            clean_data.baro_atm[1] = atof(value);
        }
        else if (strcmp(key, "velocity(m/s)") == 0)
        {
            clean_data.speed.z = atof(value);
        }
        else if (strcmp(key, "acceleration") == 0)
        {
            char *x = strtok(value, ",");
            char *y = strtok(NULL, ",");
            char *z = strtok(NULL, ",");
            clean_data.accel.x = atof(x);
            clean_data.accel.y = atof(y);
            clean_data.accel.z = atof(z);
        }
        else if (strcmp(key, "pressure(Pa)") == 0)
        {
            clean_data.baro_atm[0] = atof(value);
        }
        else if (strcmp(key, "command") == 0)
        {
            cmd = value;
        }
        token = strtok(NULL, ";");
    }
}
