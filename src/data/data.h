#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <Protocol.h>
#include "bmi08x.h"
#include "bmp3.h"
#include "adxl375.h"
#include "../flight_control/States_def.h"



/**
 * @brief A struct made only for convenience. Holds exactly
 * the same data than av_uplink_t, but the command ID is already
 * cast from uint8_t to CMD_ID to facilitate switch statements.
 */
struct UplinkCmd {
    CMD_ID id;
    uint8_t value;
};

struct SensStatus {
    uint8_t     adxl_status;
    uint8_t     adxl_aux_status;
    uint8_t     bmi_accel_status;
    uint8_t     bmi_aux_accel_status;
    uint8_t     bmi_gyro_status;
    uint8_t     bmi_aux_gyro_status;
    bmp3_status bmp_status;
    bmp3_status bmp_aux_status;
};

struct NavSensors {
    adxl375_data        adxl;
    adxl375_data        adxl_aux;
    bmi08_sensor_data_f bmi_accel;
    bmi08_sensor_data_f bmi_gyro; // Correspondance x,y,z / roll,pitch,yaw à déterminer
    bmi08_sensor_data_f bmi_aux_accel;
    bmi08_sensor_data_f bmi_aux_gyro;
    bmp3_data           bmp;
    bmp3_data           bmp_aux;

    NavSensors();
};

struct PropSensors {
    double    N2_pressure;
    double    fuel_pressure;
    double    LOX_pressure;
    double    igniter_pressure;
    double    LOX_inj_pressure;
    double    fuel_inj_pressure;
    double    chamber_pressure;
    double    fuel_level;
    double    LOX_level;
    double    N2_temperature;
    double    fuel_temperature;
    double    LOX_temperature;
    double    igniter_temperature;
    double    fuel_inj_temperature;
    double    fuel_inj_cooling_temperature;
    double    LOX_inj_temperature;
    double    chamber_temperature;

    PropSensors();
};

struct Event {
    bool   armed;
    bool   ignited;
    bool calibrated;
    bool seperated;
    bool chute_opened;
    bool chute_unreefed;
    bool command_updated;
};

struct Valves{
    bool valve1;
    bool valve2;
    bool vent3;
    bool vent4;

    bool ValvesForIgnition() const;
    bool ValvesForAbort() const;
    bool ValvesManual() const;
};

struct Vector3 {
    double x;
    double y;
    double z;
    double norm() const;
};

struct GPSCoord {
    double lat;
    double lng;
    double alt;
};

struct GPSTime {
    unsigned year;
    unsigned month;
    unsigned day;
    unsigned hour;
    unsigned minute;
    unsigned second;
    unsigned centisecond;
};

struct NavigationData {
    GPSTime   time;
    GPSCoord  position;
    Vector3   speed;
    Vector3   accel;
    Vector3   attitude;
    double    course;
    double    altitude;
    bmp3_data baro;

    NavigationData();
};

struct DataDump {
    UplinkCmd telemetry_cmd;
    SensStatus stat;
    NavSensors sens;
    PropSensors prop;
    NavigationData nav;
    Event event;
    Valves valves;
    State av_state;
};

/**
 * @brief GOAT - Global Objects Atomic Table
 * https://rocket-team.epfl.ch/competition/firehorn/systems_engineering/other/sam_homepage/avionics/software/architecture
 * 
 * Serves as the collection of all non-volatile variables.
 */
class Data {
public:
    enum GoatReg {
        /* Telemetry command */
        TLM_CMD_ID = 0x00,
        TLM_CMD_VALUE = 0x01,

        /* Navigation sensors status */
        //TODO: sensors that have 3 32 bits values should be split into 3 registers?
        NAV_SENSOR_ADXL1_STAT = 0x02,
        NAV_SENSOR_ADXL2_STAT = 0x03,
        NAV_SENSOR_BMI1_ACCEL_STAT = 0x04,
        NAV_SENSOR_BMI2_ACCEL_STAT = 0x05,
        NAV_SENSOR_BMI1_GYRO_STAT = 0x07,
        NAV_SENSOR_BMI2_GYRO_STAT = 0x08,
        NAV_SENSOR_BMP1_STAT = 0x09,
        NAV_SENSOR_BMP2_STAT = 0x0A,

        /* Raw navigation sensors data */
        NAV_SENSOR_ADXL1_DATA = 0x0B,
        NAV_SENSOR_ADXL2_DATA = 0x0C,
        NAV_SENSOR_BMI1_ACCEL_DATA = 0x0D,
        NAV_SENSOR_BMI1_GYRO_DATA = 0x0E,
        NAV_SENSOR_BMI2_ACCEL_DATA = 0x0F,
        NAV_SENSOR_BMI2_GYRO_DATA = 0x10,
        NAV_SENSOR_BMP1_DATA = 0x11,
        NAV_SENSOR_BMP2_DATA = 0x12,

        /* Propulsion sensors */
        PR_SENSOR_P_NCO = 0x13, // N2 Pressure
        PR_SENSOR_P_ETA = 0x14, // Ethanol Tank Pressure
        PR_SENSOR_P_OTA = 0x15, // Lox Tank Pressure
        PR_SENSOR_P_CIG = 0x16, // Igniter Pressure
        PR_SENSOR_P_EIN = 0x17, // Ethanol Injector Pressure
        PR_SENSOR_P_OIN = 0x18, // Lox Injector Pressure
        PR_SENSOR_P_CCC = 0x19, // Combustion Chamber Pressure
        PR_SENSOR_L_ETA = 0x1A, // Ethanol Tank Level
        PR_SENSOR_L_OTA = 0x1B, // Lox Tank Level
        PR_SENSOR_T_NCO = 0x1C, // N2 Temperature
        PR_SENSOR_T_ETA = 0x1D, // Ethanol Tank Temperature
        PR_SENSOR_T_OTA = 0x1E, // Lox Tank Temperature
        PR_SENSOR_T_CIG = 0x1F, // Igniter Temperature
        PR_SENSOR_T_EIN = 0x20, // Ethanol Injector Temperature (Sensata PTE7300)
        PR_SENSOR_T_EIN_CF = 0x21, // Ethanol Injector Cooling Fluid Tempreature (PT1000)
        PR_SENSOR_T_OIN = 0x22, // Lox Injector Temperature
        PR_SENSOR_T_CCC = 0x23,  // Combustion Chamber Temperature

        NAV_GNSS_TIME_YEAR = 0x24,
        NAV_GNSS_TIME_MONTH = 0x25,
        NAV_GNSS_TIME_DAY = 0x26,
        NAV_GNSS_TIME_HOUR = 0x27,
        NAV_GNSS_TIME_MINUTE = 0x28,
        NAV_GNSS_TIME_SECOND = 0x29,
        NAV_GNSS_TIME_CENTI = 0x2A,
        NAV_GNSS_POS_LAT = 0x2B,
        NAV_GNSS_POS_LNG = 0x2C,
        NAV_GNSS_POS_ALT = 0x2D,
        NAV_GNSS_COURSE = 0x2E,
        //TODO deconstruct EVENT and VALVES
        EVENT_ARMED = 0x2F,
        EVENT_IGNITED = 0x30,
        EVENT_CALIBRATED = 0x31,
        EVENT_SEPERATED = 0x33,
        EVENT_CHUTE_OPENED = 0x34,
        EVENT_CHUTE_UNREEFED = 0x35,
        VALVES = 0x36,
        AV_STATE = 0x37
    };

    static inline Data& get_instance() {
        static Data instance;
        return instance;
    }

    // /**
    //  * @brief Reads specific data from the GOAT.
    //  * @param reg (most likely an enum, acts like a register), specifies what data is to be read.
    //  * @return A read only pointer to the data located at the given field.
    //  */
    // const void* read(GoatReg reg);

    /**
     * @brief Writes data to the GOAT.
     * @param reg Enum specifying where to write the data.
     * @param data The data which is to be written.
     */
    void write(GoatReg reg, void* data);

    DataDump get() const;

    Data(Data const&) = delete; // Prevents copying
    void operator=(Data const&) = delete; // Prevents assignment
private:
    inline Data() {}
    inline ~Data() {}

    UplinkCmd telemetry_cmd;
    SensStatus sensors_status;
    NavSensors nav_sensors;
    PropSensors prop_sensors;
    NavigationData nav;
    Event event;
    Valves valves;
    State av_state;

};

#endif /* DATA_H */