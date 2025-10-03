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

// return true iff the content of the two adxl375_data structs is equal
bool areEqualAdxl1375_data(const adxl375_data& s1, const adxl375_data& s2) {
    return (
        s1.x == s2.x &&
        s1.y == s2.y &&
        s1.z == s2.z
    );
}

// return true iff the content of the two bmi08_sensor_data_f structs is equal
bool areEqualbmi08_sensor_data_f(const bmi08_sensor_data_f& s1, const bmi08_sensor_data_f& s2) {
    return (
        s1.x == s2.x &&
        s1.y == s2.y &&
        s1.z == s2.z
    );
}

bool areEqualValves(const Valves& s1, const Valves& s2) {
    return (
        s1.valve_dpr_vent_copv == s2.valve_dpr_vent_copv &&
        s1.valve_dpr_pressure_lox == s2.valve_dpr_pressure_lox &&
        s1.valve_dpr_pressure_fuel == s2.valve_dpr_pressure_fuel &&
        s1.valve_dpr_vent_lox == s2.valve_dpr_vent_lox &&
        s1.valve_dpr_vent_fuel == s2.valve_dpr_vent_fuel &&
        s1.valve_prb_main_lox == s2.valve_prb_main_lox &&
        s1.valve_prb_main_fuel == s2.valve_prb_main_fuel
    );
}

// return true iff the content of the two bmp3_data structs is equal
bool areEqualbmp3_data(const bmp3_data& s1, const bmp3_data& s2) {
    return (
        s1.pressure == s2.pressure &&
        s1.temperature == s2.temperature
    );
}

bool areEqualVector3(const Vector3& s1, const Vector3& s2) {
    return (
        s1.x == s2.x &&
        s1.y == s2.y &&
        s1.z == s1.z
    );
}

bool areEqualGPSCoord(const GPSCoord& s1, const GPSCoord& s2) {
    return (
        s1.alt == s2.alt &&
        s1.lat == s2.lat &&
        s1.lng == s2.lng
    );
}

bool areEqualGPSTime(const GPSTime& s1, const GPSTime& s2) {
    return (
        s1.year == s2.year &&
        s1.month == s2.month &&
        s1.day == s2.day &&
        s1.hour == s2.hour &&
        s1.minute == s2.minute &&
        s1.second == s2.second &&
        s1.centisecond == s2.centisecond
    );
}

void assertNavData(const NavigationData& s1, const NavigationData& s2) {
    // commented asserts need to be uncommented once implemented in data.cpp
    //assert(areEqualGPSTime(s1.time, s2.time));
    //assert(areEqualGPSCoord(s1.position, s2.position));
    assert(areEqualVector3(s1.position_kalman,s2.position_kalman));
    assert(areEqualVector3(s1.speed, s2.speed));
    assert(areEqualVector3(s1.accel, s2.accel));
    assert(areEqualVector3(s1.attitude, s2.attitude));
    //assert(s1.course == s2.course);
    assert(s1.altitude == s2.altitude);
    assert(areEqualbmp3_data(s1.baro, s2.baro));
    return;
}

int main(int argc, char** argv) {
    // Initialize the Goat
    Data& goatData = Data::get_instance();

    /*Tests Telemetry command*/

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

    /*Tests Navigation sensors status*/

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

    /*Tests raw navigation sensors data*/

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

    /*Tests Propulsion sensors*/

    // Test PR_SENSOR_P_NCO
    Data::GoatReg pNcoReg = Data::GoatReg::PR_SENSOR_P_NCO;
    double testPNco = 1.1;
    goatData.write(pNcoReg, &testPNco);
    dump = goatData.get();
    assert(dump.prop.N2_pressure == testPNco);

    // Test PR_SENSOR_P_ETA
    Data::GoatReg pEtaReg = Data::GoatReg::PR_SENSOR_P_ETA;
    double testPEta = 2.2;
    goatData.write(pEtaReg, &testPEta);
    dump = goatData.get();
    assert(dump.prop.fuel_pressure == testPEta);

    // Test PR_SENSOR_P_OTA
    Data::GoatReg pOtaReg = Data::GoatReg::PR_SENSOR_P_OTA;
    double testPOta = 3.3;
    goatData.write(pOtaReg, &testPOta);
    dump = goatData.get();
    assert(dump.prop.LOX_pressure == testPOta);

    // Test PR_SENSOR_P_CIG
    /*Data::GoatReg pCigReg = Data::GoatReg::PR_SENSOR_P_CIG;
    double testPCig = 4.4;
    goatData.write(pCigReg, &testPCig);
    dump = goatData.get();
    assert(dump.prop.igniter_pressure == testPCig);
*/
    // Test PR_SENSOR_P_EIN
    Data::GoatReg pEinReg = Data::GoatReg::PR_SENSOR_P_EIN;
    double testPEin = 5.5;
    goatData.write(pEinReg, &testPEin);
    dump = goatData.get();
    assert(dump.prop.fuel_inj_pressure == testPEin);

    // Test PR_SENSOR_P_OIN
    Data::GoatReg pOinReg = Data::GoatReg::PR_SENSOR_P_OIN;
    double testPOin = 6.6;
    goatData.write(pOinReg, &testPOin);
    dump = goatData.get();
    assert(dump.prop.LOX_inj_pressure == testPOin);

    // Test PR_SENSOR_P_CCC
    Data::GoatReg pCccReg = Data::GoatReg::PR_SENSOR_P_CCC;
    double testpCcc = 7.7;
    goatData.write(pCccReg, &testpCcc);
    dump = goatData.get();
    assert(dump.prop.chamber_pressure == testpCcc);

    // Test PR_SENSOR_T_NCO
    Data::GoatReg tNcoReg = Data::GoatReg::PR_SENSOR_T_NCO;
    double testtNco = 1.0;
    goatData.write(tNcoReg, &testtNco);
    dump = goatData.get();
    assert(dump.prop.N2_temperature == testtNco);

    // Test PR_SENSOR_T_ETA
    Data::GoatReg tEtaReg = Data::GoatReg::PR_SENSOR_T_ETA;
    double testtEta = 2.0;
    goatData.write(tEtaReg, &testtEta);
    dump = goatData.get();
    assert(dump.prop.fuel_temperature == testtEta);

    // Test PR_SENSOR_T_OTA
    Data::GoatReg tOtaReg = Data::GoatReg::PR_SENSOR_T_OTA;
    double testtOta = 3.0;
    goatData.write(tOtaReg, &testtOta);
    dump = goatData.get();
    assert(dump.prop.LOX_temperature == testtOta);

    // Test PR_SENSOR_T_CIG
    /*
    Data::GoatReg tCigReg = Data::GoatReg::PR_SENSOR_T_CIG;
    double testtCig = 4.0;
    goatData.write(tCigReg, &testtCig);
    dump = goatData.get();
    assert(dump.prop.igniter_temperature == testtCig);
*/
    // Test PR_SENSOR_T_EIN
    Data::GoatReg tEinReg = Data::GoatReg::PR_SENSOR_T_EIN;
    double testtEin = 5.0;
    goatData.write(tEinReg, &testtEin);
    dump = goatData.get();
    assert(dump.prop.fuel_inj_temperature == testtEin);

    // Test PR_SENSOR_T_EIN_CF
    /*
    Data::GoatReg tEinCfReg = Data::GoatReg::PR_SENSOR_T_EIN_CF;
    double testtEinCf = 6.0;
    goatData.write(tEinCfReg, &testtEinCf);
    dump = goatData.get();
    assert(dump.prop.fuel_inj_cooling_temperature == testtEinCf);
*/
    // Test PR_SENSOR_T_OIN
    /*
    Data::GoatReg tOinReg = Data::GoatReg::PR_SENSOR_T_OIN;
    double testtOin = 7.0;
    goatData.write(tOinReg, &testtOin);
    dump = goatData.get();
    assert(dump.prop.LOX_inj_temperature == testtOin);
*/
    // Test PR_SENSOR_T_CCC
    Data::GoatReg tCccReg = Data::GoatReg::PR_SENSOR_T_CCC;
    double testtCcc = 8.0;
    goatData.write(tCccReg, &testtCcc);
    dump = goatData.get();
    assert(dump.prop.chamber_temperature == testtCcc);

    /*Tests Event, Valves and State*/

    // Test EVENT_ARMED
    /*
    Data::GoatReg eventArmedReg = Data::GoatReg::EVENT_ARMED;
    bool testEventArmed = true;
    goatData.write(eventArmedReg, &testEventArmed);
    dump = goatData.get();
    assert(dump.event.dpr_ok == testEventArmed);
    */

    // Test EVENT_IGNITED
    Data::GoatReg eventIgnitedReg = Data::GoatReg::EVENT_IGNITED;
    bool testEventIgnited = true;
    goatData.write(eventIgnitedReg, &testEventIgnited);
    dump = goatData.get();
    assert(dump.event.ignited == testEventIgnited);

    // Test EVENT_CALIBRATED
    Data::GoatReg eventCalibratedReg = Data::GoatReg::EVENT_CALIBRATED;
    bool testEventCalibrated = 8.0;
    goatData.write(eventCalibratedReg, &testEventCalibrated);
    dump = goatData.get();
    assert(dump.event.calibrated == testEventCalibrated);

    // Test EVENT_SEPERATED
    Data::GoatReg eventSeparetedReg = Data::GoatReg::EVENT_SEPERATED;
    bool testEventSepareted = true;
    goatData.write(eventSeparetedReg, &testEventSepareted);
    dump = goatData.get();
    assert(dump.event.seperated == testEventSepareted);

    // Test EVENT_CHUTE_OPENED
    /*
    Data::GoatReg chuteOpenedReg = Data::GoatReg::EVENT_CHUTE_OPENED;
    bool testChuteOpened = true;
    goatData.write(chuteOpenedReg, &testChuteOpened);
    dump = goatData.get();
    assert(dump.event.chute_opened == testChuteOpened);
    */

    // Test EVENT_CHUTE_UNREEFED
    Data::GoatReg chuteUnreefedReg = Data::GoatReg::EVENT_CHUTE_UNREEFED;
    bool testChuteUnreefed = true;
    goatData.write(chuteUnreefedReg, &testChuteUnreefed);
    dump = goatData.get();
    assert(dump.event.chute_unreefed == testChuteUnreefed);

    // Test VALVES
    Data::GoatReg valvesReg = Data::GoatReg::VALVES;
    Valves testValves;
    testValves.valve_dpr_pressure_lox = 1;
    testValves.valve_dpr_pressure_fuel = 1;
    testValves.valve_dpr_vent_lox = 1;
    testValves.valve_prb_main_fuel = 1;
    goatData.write(valvesReg, &testValves);
    dump = goatData.get();
    assert(areEqualValves(dump.valves, testValves));

    // Test AV_STATE
    Data::GoatReg avStateReg = Data::GoatReg::AV_STATE;
    State testAvState = State::BURN;
    goatData.write(avStateReg, &testAvState);
    dump = goatData.get();
    assert(dump.av_state == testAvState);

    /* Test NAV_KALMAN_DATA (NavigationData) */

    Data::GoatReg navKalmanReg = Data::GoatReg::NAV_KALMAN_DATA;
    GPSTime time = {1, 2, 3, 4, 5, 6, 7};
    GPSCoord coord = {2.0, 3.0, 4.0};
    Vector3 pos = {1.0, 2.0, 3.0};
    Vector3 speed = {2.0, 3.0, 1.0};
    Vector3 accel = {3.0, 1.0, 2.0};
    Vector3 attitude = {1.1, 2.0, 3.0};
    double course = 3.2;
    double altitude = 2.3;
    bmp3_data baro = {2.3, 3.2};
    NavigationData testNavData;
    testNavData.time = time;
    testNavData.position = coord;
    testNavData.position_kalman = pos;
    testNavData.speed = speed;
    testNavData.accel = accel;
    testNavData.attitude = attitude;
    testNavData.course = course;
    testNavData.altitude = altitude;
    testNavData.baro = baro;
    goatData.write(navKalmanReg, &testNavData);
    dump = goatData.get();
    assertNavData(dump.nav, testNavData);
}
