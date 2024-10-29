#include "data.h"
#include <cassert>

int main(int argc, char** argv) {
    // Initalize a Go
    Data& goatData = Data::get_instance();

    Data::GoatReg telemetryCommand = Data::GoatReg::TLM_CMD_ID;
    int testTelemetryValue = 42;
    goatData.write(telemetryCommand, &testTelemetryValue);

    int retrievedTelemetryValue;

    DataDump dump = goatData.get();
    
    assert(dump.telemetry_cmd.id == testTelemetryValue);
}