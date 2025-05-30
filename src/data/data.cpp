// TODO: Data logging

#include "data.h"
#include "thresholds.h"

NavSensors::NavSensors()
:   adxl{0, 0, 0},
    adxl_aux{0, 0, 0},
    bmi_accel{0, 0, 0},
    bmi_gyro{0, 0, 0},
    bmi_aux_accel{0, 0, 0},
    bmi_aux_gyro{0, 0, 0},
    bmp{0, 0},
    bmp_aux{0, 0}
{}

PropSensors::PropSensors()
:   N2_pressure(0),
    fuel_pressure(0),
    LOX_pressure(0),
    fuel_level(0),
    LOX_level(0),
    igniter_pressure(0),
    LOX_inj_pressure(0),
    fuel_inj_pressure(0),
    chamber_pressure(0),
    N2_temperature(0),
    fuel_temperature(0),
    LOX_temperature(0),
    igniter_temperature(0),
    fuel_inj_temperature(0),
    fuel_inj_cooling_temperature(0),
    LOX_inj_temperature(0),
    chamber_temperature(0)
{}

NavigationData::NavigationData()
:   time{0, 0, 0, 0, 0, 0, 0},
    position{0, 0, 0},
    speed{0, 0, 0},
    accel{0, 0, 0},
    attitude{0, 0, 0},
    course(0),
    altitude(0),
    baro{0, 0}
{}

Event::Event()
:   command_updated(false),
    calibrated(false),
    dpr_eth_ready(false),
    dpr_eth_pressure_ok(false),
    dpr_lox_ready(false),
    dpr_lox_pressure_ok(false),
    prb_ready(false),
    trb_ready(false),
    ignited(false),
    seperated(false),
    chute_unreefed(false),
    ignition_failed(false)
{}


// const void* Data::read(GoatReg reg) {
//     // Big switch to read at the field given as argument
//     // Cast the void ptr to the type of data located at the given field
//     // Return the ptr, which can be later dereferenced by the caller.
// }

void Data::write(GoatReg reg, void* data) {
    // Big switch
    // Cast the void ptr to the type of data located at the given field
    // Write the data.
    switch (reg) {
        case AV_STATE:
            av_state = *reinterpret_cast<State*>(data);
            break;
        case AV_TIMESTAMP:
            av_timestamp = *reinterpret_cast<uint32_t*>(data);
            break;
        case TLM_CMD_ID:
            telemetry_cmd.id = *reinterpret_cast<CMD_ID*>(data);
            break;
        case TLM_CMD_VALUE:
            telemetry_cmd.value = *reinterpret_cast<uint8_t*>(data);
            break;
        case AV_FC_TEMPERATURE:
            av_fc_temp = *reinterpret_cast<float*>(data);
            break;
        case NAV_SENSOR_ADXL1_STAT:
            sensors_status.adxl_status = *reinterpret_cast<uint8_t*>(data);
            break;
        case NAV_SENSOR_ADXL2_STAT:
            sensors_status.adxl_aux_status = *reinterpret_cast<uint8_t*>(data);
            break;
        case NAV_SENSOR_BMI1_ACCEL_STAT:
            sensors_status.bmi_accel_status = *reinterpret_cast<uint8_t*>(data);
            break;
        case NAV_SENSOR_BMI2_ACCEL_STAT:
            sensors_status.bmi_aux_accel_status = *reinterpret_cast<uint8_t*>(data);
            break;
        case NAV_SENSOR_BMI1_GYRO_STAT:
            sensors_status.bmi_gyro_status = *reinterpret_cast<uint8_t*>(data);
            break;
        case NAV_SENSOR_BMI2_GYRO_STAT:
            sensors_status.bmi_aux_gyro_status = *reinterpret_cast<uint8_t*>(data);
            break;
        case NAV_SENSOR_BMP1_STAT:
            sensors_status.bmp_status = *reinterpret_cast<bmp3_status*>(data);
            break;
        case NAV_SENSOR_BMP2_STAT:
            sensors_status.bmp_aux_status = *reinterpret_cast<bmp3_status*>(data);
            break;
        case NAV_SENSOR_ADXL1_DATA:
            nav_sensors.adxl = *reinterpret_cast<adxl375_data*>(data);
            break;
        case NAV_SENSOR_ADXL2_DATA:
            nav_sensors.adxl_aux = *reinterpret_cast<adxl375_data*>(data);
            break;
        case NAV_SENSOR_BMI1_ACCEL_DATA:
            nav_sensors.bmi_accel = *reinterpret_cast<bmi08_sensor_data_f*>(data);
            break;
        case NAV_SENSOR_BMI2_ACCEL_DATA:
            nav_sensors.bmi_aux_accel = *reinterpret_cast<bmi08_sensor_data_f*>(data);
            break;
        case NAV_SENSOR_BMI1_GYRO_DATA:
            nav_sensors.bmi_gyro = *reinterpret_cast<bmi08_sensor_data_f*>(data);
            break;
        case NAV_SENSOR_BMI2_GYRO_DATA:
            nav_sensors.bmi_aux_gyro = *reinterpret_cast<bmi08_sensor_data_f*>(data);
            break;
        case NAV_SENSOR_BMP1_DATA:
            nav_sensors.bmp = *reinterpret_cast<bmp3_data*>(data);
            break;
        case NAV_SENSOR_BMP2_DATA:
            nav_sensors.bmp_aux = *reinterpret_cast<bmp3_data*>(data);
            break;
        case PR_SENSOR_P_NCO:
            prop_sensors.N2_pressure = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_P_ETA:
            prop_sensors.fuel_pressure = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_P_OTA:
            prop_sensors.LOX_pressure = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_P_CIG:
            prop_sensors.igniter_pressure = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_P_EIN:
            prop_sensors.fuel_inj_pressure = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_P_OIN:
            prop_sensors.LOX_inj_pressure = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_P_CCC:
            prop_sensors.chamber_pressure = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_L_ETA:
            prop_sensors.fuel_level = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_L_OTA:
            prop_sensors.LOX_level = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_NCO:
            prop_sensors.N2_temperature = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_ETA:
            prop_sensors.fuel_temperature = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_OTA:
            prop_sensors.LOX_temperature = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_CIG:
            prop_sensors.igniter_temperature = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_EIN:
            prop_sensors.fuel_inj_temperature = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_EIN_CF:
            prop_sensors.fuel_inj_cooling_temperature = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_OIN:
            prop_sensors.LOX_inj_temperature = *reinterpret_cast<double*>(data);
            break;
        case PR_SENSOR_T_CCC:
            prop_sensors.chamber_temperature = *reinterpret_cast<double*>(data);
            break;
        case VALVES:
            valves = *reinterpret_cast<Valves*>(data);
            break;
        case NAV_GNSS_TIME_YEAR:
            nav.time.year = *reinterpret_cast<unsigned*>(data);
            break;
        case NAV_GNSS_TIME_MONTH:
            nav.time.month = *reinterpret_cast<unsigned*>(data);
            break;
        case NAV_GNSS_TIME_DAY:
            nav.time.day = *reinterpret_cast<unsigned*>(data);
            break;
        case NAV_GNSS_TIME_HOUR:
            nav.time.hour = *reinterpret_cast<unsigned*>(data);
            break;
        case NAV_GNSS_TIME_MINUTE:
            nav.time.minute = *reinterpret_cast<unsigned*>(data);
            break;
        case NAV_GNSS_TIME_SECOND:
            nav.time.second = *reinterpret_cast<unsigned*>(data);
            break;
        case NAV_GNSS_TIME_CENTI:
            nav.time.centisecond = *reinterpret_cast<unsigned*>(data);
            break;
        case NAV_GNSS_POS_LAT:
            nav.position.lat = *reinterpret_cast<double*>(data);
            break;
        case NAV_GNSS_POS_LNG:
            nav.position.lng = *reinterpret_cast<double*>(data);
            break;
        case NAV_GNSS_POS_ALT:
            nav.position.alt = *reinterpret_cast<double*>(data);
            break;
        case NAV_GNSS_COURSE:
            nav.course = *reinterpret_cast<double*>(data);
            break;
        case BAT_LPB_VOLTAGE:
            bat.lpb_voltage = *reinterpret_cast<float*>(data);
            break;
        case BAT_HPB_VOLTAGE:
            bat.hpb_voltage = *reinterpret_cast<float*>(data);
            break;
        case CAM_RECORDING_SEP:
            cams_recording.cam_sep = *reinterpret_cast<bool*>(data);
            break;
        case CAM_RECORDING_UP:
            cams_recording.cam_up = *reinterpret_cast<bool*>(data);
            break;
        case CAM_RECORDING_DOWN:
            cams_recording.cam_down = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_CMD_RECEIVED:
            event.command_updated = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_CALIBRATED:
            event.calibrated = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_DPR_ETH_READY:
            event.dpr_eth_ready = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_DPR_ETH_PRESSURE_OK:
            event.dpr_eth_pressure_ok = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_DPR_LOX_READY:
            event.dpr_lox_ready = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_DPR_LOX_PRESSURE_OK:
            event.dpr_lox_pressure_ok = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_PRB_READY:
            event.prb_ready = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_TRB_READY:
            event.trb_ready = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_IGNITED:
            event.ignited = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_SEPERATED:
            event.seperated = *reinterpret_cast<bool*>(data);
            break;
        case EVENT_CHUTE_UNREEFED:
            event.chute_unreefed = *reinterpret_cast<bool*>(data);
            break;

        case NAV_KALMAN_DATA:
            const NavigationData temp = *reinterpret_cast<NavigationData*>(data);
            // only update the data given by the kalman filter
            nav.position_kalman = temp.position_kalman;
            nav.speed = temp.speed;
            nav.accel = temp.accel;
            nav.attitude = temp.attitude;
            nav.altitude = temp.altitude;
            nav.baro = temp.baro;
            break;
    }
}

DataDump Data::get() const {
    return {
        av_state,
        av_timestamp,
        telemetry_cmd,
        av_fc_temp,
        sensors_status, 
        nav_sensors, 
        prop_sensors, 
        valves,
        nav,
        bat,
        cams_recording,
        event
    };
}

bool DataDump::depressurised() const {
    return prop.N2_pressure < N2_PRESSURE_ZERO
        && prop.fuel_pressure < FUEL_PRESSURE_ZERO
        && prop.LOX_pressure < LOX_PRESSURE_ZERO
        && prop.fuel_inj_pressure < INJECTOR_PRESSURE_ZERO
        && prop.LOX_inj_pressure < INJECTOR_PRESSURE_ZERO
        && prop.chamber_pressure < CHAMBER_PRESSURE_ZERO;
}

bool Valves::ValvesForIgnition() const {
    //TODO:must change to fit real wanted state of valves
    return valve1 && valve2 && vent3 && vent4;
}

bool Valves::ValvesManual() const {
    return valve1 && valve2 && vent3 && vent4;
}
