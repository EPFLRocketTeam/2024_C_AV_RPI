#include "data.h"
#include <cassert>

// return true iff the content of the two bmp3_status structs is equal
bool areEqualBmp3_status(const bmp3_status& s1, const bmp3_status& s2) {
    return (
        s1.intr.fifo_wm == s2.intr.fifo_wm &&
        s1.intr.fifo_full == s2.intr.fifo_full &&
        s1.intr.drdy == s2.intr.drdy &&
        s1.sensor.cmd_rdy == s2.sensor.cmd_rdy &&
        s1.sensor.drdy_press == s2.sensor.drdy_press &&
        s1.sensor.drdy_temp == s2.sensor.drdy_temp &&
        s1.err.fatal == s2.err.fatal &&
        s1.err.cmd == s2.err.cmd &&
        s1.err.conf == s2.err.conf &&
        s1.pwr_on_rst == s2.pwr_on_rst
    );
}

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
    uint8_t testAdxl2Status = 2;
    goatData.write(adxl2StatusReg, &testAdxl2Status);
    dump = goatData.get();
    assert(dump.stat.adxl_aux_status == testAdxl2Status);

    // Test NAV_SENSOR_BMI1_ACCEL_STAT
    Data::GoatReg bmi1AccelStatusReg = Data::GoatReg::NAV_SENSOR_BMI1_ACCEL_STAT;
    uint8_t testbmi1AccelStatus = 3;
    goatData.write(bmi1AccelStatusReg, &testbmi1AccelStatus);
    dump = goatData.get();
    assert(dump.stat.bmi_accel_status == testbmi1AccelStatus);

    // Test NAV_SENSOR_BMI2_ACCEL_STAT
    Data::GoatReg bmi2AccelStatusReg = Data::GoatReg::NAV_SENSOR_BMI2_ACCEL_STAT;
    uint8_t testbmi2AccelStatus = 4;
    goatData.write(bmi2AccelStatusReg, &testbmi2AccelStatus);
    dump = goatData.get();
    assert(dump.stat.bmi_aux_accel_status == testbmi2AccelStatus);

    // Test NAV_SENSOR_BMI1_GYRO_STAT
    Data::GoatReg bmi1GyroStatusReg = Data::GoatReg::NAV_SENSOR_BMI1_GYRO_STAT;
    uint8_t testbmi1GyroStatus = 5;
    goatData.write(bmi1GyroStatusReg, &testbmi1GyroStatus);
    dump = goatData.get();
    assert(dump.stat.bmi_gyro_status == testbmi1GyroStatus);

    // Test NAV_SENSOR_BMI2_GYRO_STAT
    Data::GoatReg bmi2GyroStatusReg = Data::GoatReg::NAV_SENSOR_BMI2_GYRO_STAT;
    uint8_t testbmi2GyroStatus = 6;
    goatData.write(bmi2GyroStatusReg, &testbmi2GyroStatus);
    dump = goatData.get();
    assert(dump.stat.bmi_aux_gyro_status == testbmi2GyroStatus);

    // Test NAV_SENSOR_BMP1_STAT
    Data::GoatReg bmp1StatusReg = Data::GoatReg::NAV_SENSOR_BMP1_STAT;
    bmp3_status testbmp1Status = {{0,1,2},{3,4,5},{6,7,8},7};
    goatData.write(bmp1StatusReg, &testbmp1Status);
    dump = goatData.get();
    assert(areEqualBmp3_status(dump.stat.bmp_status, testbmp1Status));

    // Test NAV_SENSOR_BMP2_STAT
    Data::GoatReg bmp2StatusReg = Data::GoatReg::NAV_SENSOR_BMP2_STAT;
    bmp3_status testbmp2Status = {{3,4,5},{6,7,8},{0,1,2},8};
    goatData.write(bmp2StatusReg, &testbmp2Status);
    dump = goatData.get();
    assert(areEqualBmp3_status(dump.stat.bmp_status, testbmp2Status));
}