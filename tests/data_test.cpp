#include "data.h"
#include <cassert>

int main(int argc, char **argv)
{
    // Initialize the Goat
    Data &goatData = Data::get_instance();

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


    // Test ADXL375 
    adxl375_data testAdxl1Data = {0.1f, 0.2f, 0.3f};
    Data::GoatReg adxl1DataReg = Data::GoatReg::NAV_SENSOR_ADXL1_DATA;
    goatData.write(adxl1DataReg, &testAdxl1Data);

    dump = goatData.get();
    assert(dump.sens.adxl.x == testAdxl1Data.x);
    assert(dump.sens.adxl.y == testAdxl1Data.y);
    assert(dump.sens.adxl.z == testAdxl1Data.z);


    // Test BMI
    bmi08_sensor_data_f testBmi1Accel = {1.1f, 2.2f, 3.3f};
    Data::GoatReg bmi1AccelReg = Data::GoatReg::NAV_SENSOR_BMI1_ACCEL_DATA;
    goatData.write(bmi1AccelReg, &testBmi1Accel);

    dump = goatData.get();
    assert(dump.sens.bmi_accel.x == testBmi1Accel.x);
    assert(dump.sens.bmi_accel.y == testBmi1Accel.y);
    assert(dump.sens.bmi_accel.z == testBmi1Accel.z);

    // Test  BMP
    bmp3_data testBmp1Data = {101325.0f, 25.0f};
    Data::GoatReg bmp1DataReg = Data::GoatReg::NAV_SENSOR_BMP1_DATA;
    goatData.write(bmp1DataReg, &testBmp1Data);

    dump = goatData.get();
    assert(dump.sens.bmp.pressure == testBmp1Data.pressure);
    assert(dump.sens.bmp.temperature == testBmp1Data.temperature);
}