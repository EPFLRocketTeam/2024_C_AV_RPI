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

bool areEqualAdxl1375_data(const adxl375_data& s1, const adxl375_data& s2) {
    return (
        s1.x == s2.x &&
        s1.y == s2.y &&
        s1.z == s2.z
    );
}

bool areEqualbmi08_sensor_data_f(const bmi08_sensor_data_f& s1, const bmi08_sensor_data_f& s2) {
    return (
        s1.x == s2.x &&
        s1.y == s2.y &&
        s1.z == s2.z
    );
}

bool areEqualbmp3_data(const bmp3_data& s1, const bmp3_data& s2) {
    return (
        s1.pressure == s2.pressure &&
        s1.temperature == s2.temperature
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
    assert(areEqualBmp3_status(dump.stat.bmp_aux_status, testbmp2Status));

    // Test NAV_SENSOR_ADXL1_DATA
    Data::GoatReg adxl1DataReg = Data::GoatReg::NAV_SENSOR_ADXL1_DATA;
    adxl375_data testadxl1Data = {1.5, 2.6, 3.7};
    goatData.write(adxl1DataReg, &testadxl1Data);
    dump = goatData.get();
    assert(areEqualAdxl1375_data(dump.sens.adxl, testadxl1Data));

    // Test NAV_SENSOR_ADXL2_DATA
    Data::GoatReg adxl2DataReg = Data::GoatReg::NAV_SENSOR_ADXL2_DATA;
    adxl375_data testadxl2Data = {3.7, 1.5, 2.6};
    goatData.write(adxl2DataReg, &testadxl2Data);
    dump = goatData.get();
    assert(areEqualAdxl1375_data(dump.sens.adxl_aux, testadxl2Data));

    // Test NAV_SENSOR_BMI1_ACCEL_DATA
    Data::GoatReg bmi1AccelDataReg = Data::GoatReg::NAV_SENSOR_BMI1_ACCEL_DATA;
    bmi08_sensor_data_f testBmi1AccelData = {3.7, 1.5, 2.6};
    goatData.write(bmi1AccelDataReg, &testBmi1AccelData);
    dump = goatData.get();
    assert(areEqualbmi08_sensor_data_f(dump.sens.bmi_accel, testBmi1AccelData));

    // Test NAV_SENSOR_BMI1_GYRO_DATA
    Data::GoatReg bmi1GyroDataReg = Data::GoatReg::NAV_SENSOR_BMI1_GYRO_DATA;
    bmi08_sensor_data_f testBmi1GyroData = {2.6, 3.7, 1.5};
    goatData.write(bmi1GyroDataReg, &testBmi1GyroData);
    dump = goatData.get();
    assert(areEqualbmi08_sensor_data_f(dump.sens.bmi_gyro, testBmi1GyroData));

    // Test NAV_SENSOR_BMI2_ACCEL_DATA
    Data::GoatReg bmi2AccelDataReg = Data::GoatReg::NAV_SENSOR_BMI2_ACCEL_DATA;
    bmi08_sensor_data_f testBmi2AccelData = {2.6, 3.7, 1.5};
    goatData.write(bmi2AccelDataReg, &testBmi2AccelData);
    dump = goatData.get();
    assert(areEqualbmi08_sensor_data_f(dump.sens.bmi_aux_accel, testBmi2AccelData));

    // Test NAV_SENSOR_BMI2_GYRO_DATA
    Data::GoatReg bmi2GyroDataReg = Data::GoatReg::NAV_SENSOR_BMI2_GYRO_DATA;
    bmi08_sensor_data_f testBmi2GyroData = {2.6, 3.7, 1.5};
    goatData.write(bmi2GyroDataReg, &testBmi2GyroData);
    dump = goatData.get();
    assert(areEqualbmi08_sensor_data_f(dump.sens.bmi_aux_gyro, testBmi2GyroData));

    // Test NAV_SENSOR_BMP1_DATA
    Data::GoatReg bmp1DataReg = Data::GoatReg::NAV_SENSOR_BMP1_DATA;
    bmp3_data testBmp1Data = {1.2, 2.3};
    goatData.write(bmp1DataReg, &testBmp1Data);
    dump = goatData.get();
    assert(areEqualbmp3_data(dump.sens.bmp, testBmp1Data));

    // Test NAV_SENSOR_BMP2_DATA
    Data::GoatReg bmp2DataReg = Data::GoatReg::NAV_SENSOR_BMP2_DATA;
    bmp3_data testBmp2Data = {2.1, 3.2};
    goatData.write(bmp2DataReg, &testBmp2Data);
    dump = goatData.get();
    assert(areEqualbmp3_data(dump.sens.bmp_aux, testBmp2Data));
}