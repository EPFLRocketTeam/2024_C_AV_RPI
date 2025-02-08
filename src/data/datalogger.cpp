//
// Created by marin on 08.02.2025.
//

#include "datalogger.h"
#include <iostream>
#include <sstream>


Datalogger::Datalogger(std::string path, DataDump &dump):path(path),csvFile(path) {
    // Open a CSV file for writing
    if (!csvFile.is_open()) {
        std::cerr << "Error opening CSV file for writing." << std::endl;
        return;
    }

    // Write the header row (adjust column names as needed)
    csvFile << "telemetry_cmd_id,telemetry_cmd_value,"
            << "adxl_status,adxl_aux_status,bmi_accel_status,bmi_aux_accel_status,"
            << "bmi_gyro_status,bmi_aux_gyro_status,bmp_status_err_conf,bmp_status_err_cmd,bmp_status_err_fatal,bmp_status_sensor_cmd_rdy,bmp_status_sensor_drdy_press,bmp_status_sensor_drdy_temp,bmp_aux_status_err_conf,bmp_aux_status_err_cmd,bmp_aux_status_err_fatal,bmp_aux_status_sensor_cmd_rdy,bmp_aux_status_sensor_drdy_press,bmp_aux_status_sensor_drdy_temp,"
            << "adxl_x,adxl_y,adxl_z,"
            << "adxl_aux_x,adxl_aux_y,adxl_aux_z,"
            << "bmi_accel_x,bmi_accel_y,bmi_accel_z,"
            << "bmi_gyro_x,bmi_gyro_y,bmi_gyro_z,"
            << "bmi_aux_accel_x,bmi_aux_accel_y,bmi_aux_accel_z,"
            << "bmi_aux_gyro_x,bmi_aux_gyro_y,bmi_aux_gyro_z,"
            << "bmp_pressure,bmp_temperature,"
            << "bmp_aux_pressure,bmp_aux_temperature,"
            << "N2_pressure,fuel_pressure,LOX_pressure,igniter_pressure,"
            << "LOX_inj_pressure,fuel_inj_pressure,chamber_pressure,"
            << "fuel_level,LOX_level,N2_temperature,fuel_temperature,"
            << "LOX_temperature,igniter_temperature,fuel_inj_temperature,"
            << "fuel_inj_cooling_temperature,LOX_inj_temperature,chamber_temperature,"
            << "gps_year,gps_month,gps_day,gps_hour,gps_minute,gps_second,gps_centisecond,"
            << "gps_lat,gps_lng,gps_alt,"
            << "speed_x,speed_y,speed_z,"
            << "accel_x,accel_y,accel_z,"
            << "attitude_x,attitude_y,attitude_z,"
            << "course,altitude,"
            << "baro_pressure,baro_temperature,"
            << "dpr_ok,ignited,calibrated,seperated,chute_opened,chute_unreefed,"
            << "ignition_failed,catastrophic_failure,"
            << "valve1,valve2,vent3,vent4,"
            << "av_state" << std::endl;

    // In your application’s main loop, you would get the latest DataDump.
    // For this example, we simulate one iteration.

    auto str=Datalogger::conv(dump);

    csvFile << str << std::endl;


    // In a real logger, you would likely do this repeatedly (e.g., inside a timed loop)
    // and flush or close the file when done.
    csvFile.flush();
}

Datalogger::~Datalogger() {
    csvFile.close();
}

void bmp3StatusToCSV(const bmp3_status &status ,std::ostringstream &oss) {
    oss << static_cast<int>(status.err.conf)  << ","
        << static_cast<int>(status.err.cmd) << ","
        << static_cast<int>(status.err.fatal) << ","
        << static_cast<int>(status.sensor.cmd_rdy) << ","
        << static_cast<int>(status.sensor.drdy_press) << ","
        << static_cast<int>(status.sensor.drdy_temp) << ",";
}

void vector3ToCSV(const Vector3 &vec ,std::ostringstream &oss){
    oss << static_cast<int>(vec.x) << ","
        << static_cast<int>(vec.y) << ","
        << static_cast<int>(vec.z) << ",";
}



int Datalogger::write(std::string row) {

    if (!csvFile.is_open()) {
        std::cerr << "Error opening CSV file for writing." << std::endl;
        return 1;
    }
    csvFile<< row << std::endl;
    csvFile.flush();
    return 0;

}




std::string Datalogger::conv(const DataDump &dump) {
    std::ostringstream oss;

    // Telemetry command (casting to int for clarity in CSV)
    oss << static_cast<int>(dump.telemetry_cmd.id) << ","
        << static_cast<int>(dump.telemetry_cmd.value) << ",";

    // Sensors status
    oss << static_cast<int>(dump.stat.adxl_status) << ","
        << static_cast<int>(dump.stat.adxl_aux_status) << ","
        << static_cast<int>(dump.stat.bmi_accel_status) << ","
        << static_cast<int>(dump.stat.bmi_aux_accel_status) << ","
        << static_cast<int>(dump.stat.bmi_gyro_status) << ","
        << static_cast<int>(dump.stat.bmi_aux_gyro_status) << ",";
        bmp3StatusToCSV(dump.stat.bmp_status,oss);
        bmp3StatusToCSV(dump.stat.bmp_aux_status,oss);


    // Navigation sensors
    // Assuming adxl375_data and bmi08_sensor_data_f have members x, y, z
    oss << dump.sens.adxl.x << "," << dump.sens.adxl.y << "," << dump.sens.adxl.z << ",";
    oss << dump.sens.adxl_aux.x << "," << dump.sens.adxl_aux.y << "," << dump.sens.adxl_aux.z << ",";
    oss << dump.sens.bmi_accel.x << "," << dump.sens.bmi_accel.y << "," << dump.sens.bmi_accel.z << ",";
    oss << dump.sens.bmi_gyro.x << "," << dump.sens.bmi_gyro.y << "," << dump.sens.bmi_gyro.z << ",";
    oss << dump.sens.bmi_aux_accel.x << "," << dump.sens.bmi_aux_accel.y << "," << dump.sens.bmi_aux_accel.z << ",";
    oss << dump.sens.bmi_aux_gyro.x << "," << dump.sens.bmi_aux_gyro.y << "," << dump.sens.bmi_aux_gyro.z << ",";

    // For bmp3_data, assume fields like pressure and temperature
    oss << dump.sens.bmp.pressure << "," << dump.sens.bmp.temperature << ",";
    oss << dump.sens.bmp_aux.pressure << "," << dump.sens.bmp_aux.temperature << ",";

    // Propulsion sensors (all doubles)
    oss << dump.prop.N2_pressure << ","
        << dump.prop.fuel_pressure << ","
        << dump.prop.LOX_pressure << ","
        << dump.prop.igniter_pressure << ","
        << dump.prop.LOX_inj_pressure << ","
        << dump.prop.fuel_inj_pressure << ","
        << dump.prop.chamber_pressure << ","
        << dump.prop.fuel_level << ","
        << dump.prop.LOX_level << ","
        << dump.prop.N2_temperature << ","
        << dump.prop.fuel_temperature << ","
        << dump.prop.LOX_temperature << ","
        << dump.prop.igniter_temperature << ","
        << dump.prop.fuel_inj_temperature << ","
        << dump.prop.fuel_inj_cooling_temperature << ","
        << dump.prop.LOX_inj_temperature << ","
        << dump.prop.chamber_temperature << ",";

    // Navigation data: time and position
    oss << dump.nav.time.year << ","
        << dump.nav.time.month << ","
        << dump.nav.time.day << ","
        << dump.nav.time.hour << ","
        << dump.nav.time.minute << ","
        << dump.nav.time.second << ","
        << dump.nav.time.centisecond << ",";
    oss << dump.nav.position.lat << ","
        << dump.nav.position.lng << ","
        << dump.nav.position.alt << ",";

    // Navigation data: speed, acceleration, and attitude using our helper
    vector3ToCSV(dump.nav.speed,oss);
    vector3ToCSV(dump.nav.accel,oss);
    vector3ToCSV(dump.nav.attitude,oss);

    // Course and altitude (double values)
    oss << dump.nav.course << ","
        << dump.nav.altitude << ",";

    // Barometric sensor data (assumed bmp3_data with pressure and temperature)
    oss << dump.nav.baro.pressure << ","
        << dump.nav.baro.temperature << ",";

    // Event flags (booleans: output as 0/1)
    oss << dump.event.dpr_ok << ","
        << dump.event.ignited << ","
        << dump.event.calibrated << ","
        << dump.event.seperated << ","
        << dump.event.chute_opened << ","
        << dump.event.chute_unreefed << ","
        << dump.event.ignition_failed << ","
        << dump.event.catastrophic_failure << ",";

    // Valves state (booleans: output as 0/1)
    oss << dump.valves.valve1 << ","
        << dump.valves.valve2 << ","
        << dump.valves.vent3 << ","
        << dump.valves.vent4 << ",";

    // AV state (cast to int)
    oss << static_cast<int>(dump.av_state);

    return oss.str();
}

