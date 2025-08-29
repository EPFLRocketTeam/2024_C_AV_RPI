#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <Protocol.h>
#include "bmi08_defs.h"
#include "bmp3_defs.h"
#include "adxl375.h"
#include "math.h"

enum class State
{
    INIT,
    CALIBRATION,
    FILLING,
    ARMED,
    PRESSURIZED,
    ABORT_ON_GROUND,
    IGNITION,
    BURN,
    ASCENT,
    LANDED,
    DESCENT,
    ABORT_IN_FLIGHT
};

/**
 * @brief A struct made only for convenience. Holds exactly
 * the same data than av_uplink_t, but the command ID is already
 * cast from uint8_t to CMD_ID to facilitate switch statements.
 */
struct UplinkCmd {
    uint8_t id;
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

// TODO: merge PropSensors and Valves into Prop struct
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
    //TODO: should be changed ounce intranet with PRB_STATE is resolved
    uint32_t  PRB_state;

    PropSensors();
};

struct Valves{
    bool valve_dpr_pressure_lox;
    bool valve_dpr_pressure_fuel;
    bool valve_dpr_vent_copv;
    bool valve_dpr_vent_lox;
    bool valve_dpr_vent_fuel;
    bool valve_prb_main_lox;
    bool valve_prb_main_fuel;

    Valves();
};

struct Vector3 {
    double x;
    double y;
    double z;
    
    inline double norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }
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
    //referentiel earth
    Vector3   position_kalman;
    Vector3   speed;
    //ref of accel TBD !!!!
    Vector3   accel;
    Vector3   attitude; // Quaternion vector q = {w, x, y, z} -> {x,y,z}
    double    course;
    double    altitude;
    bmp3_data baro;

    NavigationData();
};

struct Batteries {
    float lpb_voltage;
    float lpb_current;
    float hpb_voltage;
    float hpb_current;
};

struct CamsRecording {
    bool cam_sep;
    bool cam_up;
    bool cam_down;
};

struct Event {
    bool command_updated;
    bool calibrated;
    bool dpr_eth_ready;
    bool dpr_eth_pressure_ok;
    bool dpr_lox_ready;
    bool dpr_lox_pressure_ok;
    bool prb_ready;
    bool trb_ready;
    bool ignited;
    bool engine_cut_off;
    bool seperated;
    bool chute_unreefed;
    // FILLING state this resets to 0
    bool ignition_failed;

    // will have to be discussed in interface meeting w/ prop
    // transition b/w ARMED and ERRORGROUND states
    // each tank should be checked for catastrophic failure
    bool catastrophic_failure;

    Event();
};

struct DataDump {
    State av_state;
    uint32_t av_timestamp;
    UplinkCmd telemetry_cmd;
    float av_fc_temp;
    SensStatus stat;
    NavSensors sens;
    PropSensors prop;
    Valves valves;
    NavigationData nav;
    Batteries bat;
    CamsRecording cams_recording;
    Event event;

    // TODO: move to PR_board.check_policy
    bool depressurised() const;
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
        AV_STATE = 0x00,
        AV_TIMESTAMP, // milliseconds since init

        /* Telemetry command */
        TLM_CMD_ID,
        TLM_CMD_VALUE,

        /* CM4 temperature */
        AV_FC_TEMPERATURE,

        /* Navigation sensors status */
        //TODO: sensors that have 3 32 bits values should be split into 3 registers?
        NAV_SENSOR_ADXL1_STAT,
        NAV_SENSOR_ADXL2_STAT,
        NAV_SENSOR_BMI1_ACCEL_STAT,
        NAV_SENSOR_BMI2_ACCEL_STAT,
        NAV_SENSOR_BMI1_GYRO_STAT,
        NAV_SENSOR_BMI2_GYRO_STAT,
        NAV_SENSOR_BMP1_STAT,
        NAV_SENSOR_BMP2_STAT,

        /* Raw navigation sensors data */
        NAV_SENSOR_ADXL1_DATA,
        NAV_SENSOR_ADXL2_DATA,
        NAV_SENSOR_BMI1_ACCEL_DATA,
        NAV_SENSOR_BMI1_GYRO_DATA,
        NAV_SENSOR_BMI2_ACCEL_DATA,
        NAV_SENSOR_BMI2_GYRO_DATA,
        NAV_SENSOR_BMP1_DATA,
        NAV_SENSOR_BMP2_DATA,

        /* GPS data */
        NAV_GNSS_TIME_YEAR,
        NAV_GNSS_TIME_MONTH,
        NAV_GNSS_TIME_DAY,
        NAV_GNSS_TIME_HOUR,
        NAV_GNSS_TIME_MINUTE,
        NAV_GNSS_TIME_SECOND,
        NAV_GNSS_TIME_CENTI,
        NAV_GNSS_POS_LAT,
        NAV_GNSS_POS_LNG,
        NAV_GNSS_POS_ALT,
        NAV_GNSS_COURSE,
      
        NAV_KALMAN_DATA,

        /* Propulsion sensors */
        PR_SENSOR_P_NCO, // N2 Pressure
        PR_SENSOR_P_ETA, // Ethanol Tank Pressure
        PR_SENSOR_P_OTA, // Lox Tank Pressure
        PR_SENSOR_P_CIG, // Igniter Pressure
        PR_SENSOR_P_EIN, // Ethanol Injector Pressure
        PR_SENSOR_P_OIN, // Lox Injector Pressure
        PR_SENSOR_P_CCC, // Combustion Chamber Pressure
        PR_SENSOR_L_ETA, // Ethanol Tank Level
        PR_SENSOR_L_OTA, // Lox Tank Level
        PR_SENSOR_T_NCO, // N2 Temperature
        PR_SENSOR_T_ETA, // Ethanol Tank Temperature
        PR_SENSOR_T_OTA, // Lox Tank Temperature
        PR_SENSOR_T_CIG, // Igniter Temperature
        PR_SENSOR_T_EIN , // Ethanol Injector Temperature (Sensata PTE7300)
        PR_SENSOR_T_EIN_CF, // Ethanol Injector Cooling Fluid Tempreature (PT1000)
        PR_SENSOR_T_OIN, // Lox Injector Temperature
        PR_SENSOR_T_CCC,  // Combustion Chamber Temperature

        PR_BOARD_FSM_STATE, // PRB Finite State Machine State
        VALVES,

        /* Batteries status */
        BAT_LPB_VOLTAGE,
        BAT_LPB_CURRENT,
        BAT_HPB_VOLTAGE,
        BAT_HPB_CURRENT,

        /* Cameras recording status */
        CAM_RECORDING_SEP,
        CAM_RECORDING_UP,
        CAM_RECORDING_DOWN,

        /* Events */
        EVENT_CMD_RECEIVED,
        EVENT_CALIBRATED,
        EVENT_DPR_ETH_READY,
        EVENT_DPR_ETH_PRESSURE_OK,
        EVENT_DPR_LOX_READY,
        EVENT_DPR_LOX_PRESSURE_OK,
        EVENT_PRB_READY,
        EVENT_TRB_READY,
        EVENT_IGNITED,
        EVENT_ENGINE_CUT_OFF,
        EVENT_SEPERATED,
        EVENT_CHUTE_OPENED,
        EVENT_CHUTE_UNREEFED,
        EVENT_IGNITION_FAILED
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

    State av_state;
    uint32_t av_timestamp;
    UplinkCmd telemetry_cmd;
    float av_fc_temp;
    SensStatus sensors_status;
    NavSensors nav_sensors;
    PropSensors prop_sensors;
    Valves valves;
    NavigationData nav;
    Batteries bat;
    CamsRecording cams_recording;
    Event event;
};

#endif /* DATA_H */
