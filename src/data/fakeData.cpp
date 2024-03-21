
#include "../include/data/fakeData.h"

FakeData::FakeData() {
    raw_data = SensRaw();
    clean_data = SensFiltered();
    status = SensStatus();
}

FakeData::~FakeData() {
}

void FakeData::set_raw_data(SensRaw data) {
    raw_data = data;
}

void FakeData::set_clean_data(char* data) {
    // parse -> altitude(m):165.0;velocity(m/s):0.0;Time(ms):4446.0;acceleration: (0.1569064, 8.394492399999999, -5.962443199999999);pressure(Pa):1005.68780646843;command:0
    char* token = strtok(data, ";");
    while (token != NULL) {
        char* key = strtok(token, ":");
        char* value = strtok(NULL, ":");
        if (strcmp(key, "altitude(m)") == 0) {
            clean_data.baro.pressure = atof(value);
        } else if (strcmp(key, "velocity(m/s)") == 0) {
            clean_data.speed.z = atof(value);
        } else if (strcmp(key, "acceleration") == 0) {
            char* x = strtok(value, ",");
            char* y = strtok(NULL, ",");
            char* z = strtok(NULL, ",");
            clean_data.accel.x = atof(x);
            clean_data.accel.y = atof(y);
            clean_data.accel.z = atof(z);
        } else if (strcmp(key, "pressure(Pa)") == 0) {
            clean_data.baro.pressure = atof(value);
        } else if (strcmp(key, "command") == 0) {
            UPLink cmd;
            cmd.id = CMD_ID::CMD_ID_IGNITER;
            cmd.value = atoi(value);
            set_cmd(cmd);
        }
        token = strtok(NULL, ";");
    }

}

void FakeData::set_status(SensStatus status) {
    status = status;
}