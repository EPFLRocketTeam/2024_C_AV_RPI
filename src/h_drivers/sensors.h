// Paradigm:
// Either get all sensors status/values, then write them to the GOAT
// Or implement in each sensor driver to do it directly on its own.
// Either way, do we still need to store sensors status, raw navigation values,
// and filtered global sensors values as members of this class ? Since we have the GOAT,
// can't we directly write them to it upon sensors acquisition, i.e. in Sensors::update() ?
// This typically would look like:
//  Data::get_instance().write(ADXL1_RAW_DATA, adxl1.get_data());
// That way, Sensors is just a wrapper to manage the sensor drivers, without storing anything.

#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include "data.h"
#include "kalman.h"
#include "telecom.h"
#include "trigger_board.h"
#include "PR_board.h"

#ifdef MOCK_SENSORS_ENABLED
#include "MockAdxl375.h"
#include "MockBmi08.h"
#include "MockBmp390.h"
#include "MockIna228.h"
#include "MockTmp1075.h"
#else
#include "adxl375.h"
#include "bmi08.h"
#include "bmp3.h"
#include "INA228.h"
#include "TMP1075.h"
#endif

#include "I2CGPS.h"
#include "TinyGPS++.h"
#include "tsdb.h"
#include "h_driver.h"

#include <optional>
class Sensors
{
public:
    Sensors();
    ~Sensors();

    void check_policy(const DataDump &dump, uint32_t delta_ms);
    void calibrate();
    bool update();

private:
#ifdef MOCK_SENSORS_ENABLED
    MockAdxl375 adxl1, adxl2;

    MockBmi08 bmi1, bmi2;
    MockBmp390 bmp1, bmp2;

    MockTmp1075 tmp1075;
    MockIna228 ina_lpb;
    MockIna228 ina_hpb;
#else
    Adxl375 adxl1, adxl2;
    Bmi088 bmi1, bmi2;
    Bmp390 bmp1, bmp2;

    TMP1075 tmp1075;
    INA228 ina_lpb;
    INA228 ina_hpb;

#endif

    I2CGPS i2cgps;
    TinyGPSPlus gps;

    Kalman kalman;

    TDB *tdb = nullptr;
    bool simulation_mode = false;

    std::optional<TimeSeries> adxl1_x, adxl1_y, adxl1_z;
    std::optional<TimeSeries> adxl2_x, adxl2_y, adxl2_z;

    std::optional<TimeSeries> bmp1_p, bmp1_t;
    std::optional<TimeSeries> bmi1_acc_x, bmi1_acc_y, bmi1_acc_z;
    std::optional<TimeSeries> bmi1_gyro_x, bmi1_gyro_y, bmi1_gyro_z;
    std::optional<TimeSeries> bmi2_acc_x, bmi2_acc_y, bmi2_acc_z;
    std::optional<TimeSeries> bmi2_gyro_x, bmi2_gyro_y, bmi2_gyro_z;
    std::optional<TimeSeries> bmp2_p, bmp2_t;

    // SensStatus status;
    // SensRaw raw_data;
    // SensFiltered clean_data;

    // Read sensors status
    void update_status();
};

#endif /* SENSORS_H */
