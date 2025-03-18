#include "data.h"
#include <cassert>

int main(int argc, char** argv) {
    // Initialize the Goat
    Data& goatData = Data::get_instance();

    // Test TLM_CMD_ID
    Data::GoatReg telemetryCommand = Data::GoatReg::TLM_CMD_ID;
    int testTelemetryValue = 42;
    goatData.write(telemetryCommand, &testTelemetryValue);
    DataDump dump = goatData.get();
    assert(dump.telemetry_cmd.id == testTelemetryValue);

    // Test TLM_CMD_VALUE
    Data::GoatReg telemetryValueReg = Data::GoatReg::TLM_CMD_VALUE;
    uint8_t testTelemetryCmdValue = 128;
    goatData.write(telemetryValueReg, &testTelemetryCmdValue);
    dump = goatData.get();
    assert(dump.telemetry_cmd.value == testTelemetryCmdValue);

    // Test NAV_SENSOR_ADXL1_STAT
    Data::GoatReg adxl1StatusReg = Data::GoatReg::NAV_SENSOR_ADXL1_STAT;
    uint8_t testAdxl1Status = 1;
    goatData.write(adxl1StatusReg, &testAdxl1Status);
    dump = goatData.get();
    assert(dump.stat.adxl_status == testAdxl1Status);

    // Test NAV_SENSOR_ADXL2_STAT
    Data::GoatReg adxl2StatusReg = Data::GoatReg::NAV_SENSOR_ADXL2_STAT;
    uint8_t testAdxl2Status = 1;
    goatData.write(adxl2StatusReg, &testAdxl2Status);
    dump = goatData.get();
    assert(dump.stat.adxl_aux_status == testAdxl2Status);

}