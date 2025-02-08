//
// Created by marin on 08.02.2025.
//

#include <cassert>
#include "data.h"
#include "datalogger.h"
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char** argv) {
    // __FILE__ expands to the current source file's path.
    // Get the parent directory of this source file.
    fs::path sourcePath(__FILE__);
    fs::path sourceDir = sourcePath.parent_path();
    
    // Build a path relative to the source file directory.
    // For example, if you have a "logs" directory in the same folder as your source file:
    fs::path logFilePath = sourceDir / "logs" / "log_test.csv";
    DataDump dataDump{};

    //set the values of the dataDump all to 0

    Datalogger datalogger = Datalogger(logFilePath,dataDump);
    //check if the values are written to the file
    std::ifstream file(logFilePath);
    std::string line;
    std::getline(file, line);


    //check if the first line is the header

    assert(line == "telemetry_cmd_id,telemetry_cmd_value,"
            "adxl_status,adxl_aux_status,bmi_accel_status,bmi_aux_accel_status,"
            "bmi_gyro_status,bmi_aux_gyro_status,bmp_status_err_conf,bmp_status_err_cmd,bmp_status_err_fatal,bmp_status_sensor_cmd_rdy,bmp_status_sensor_drdy_press,bmp_status_sensor_drdy_temp,bmp_aux_status_err_conf,bmp_aux_status_err_cmd,bmp_aux_status_err_fatal,bmp_aux_status_sensor_cmd_rdy,bmp_aux_status_sensor_drdy_press,bmp_aux_status_sensor_drdy_temp,"
            "adxl_x,adxl_y,adxl_z,"
            "adxl_aux_x,adxl_aux_y,adxl_aux_z,"
            "bmi_accel_x,bmi_accel_y,bmi_accel_z,"
            "bmi_gyro_x,bmi_gyro_y,bmi_gyro_z,"
            "bmi_aux_accel_x,bmi_aux_accel_y,bmi_aux_accel_z,"
            "bmi_aux_gyro_x,bmi_aux_gyro_y,bmi_aux_gyro_z,"
            "bmp_pressure,bmp_temperature,"
            "bmp_aux_pressure,bmp_aux_temperature,"
            "N2_pressure,fuel_pressure,LOX_pressure,igniter_pressure,"
            "LOX_inj_pressure,fuel_inj_pressure,chamber_pressure,"
            "fuel_level,LOX_level,N2_temperature,fuel_temperature,"
            "LOX_temperature,igniter_temperature,fuel_inj_temperature,"
            "fuel_inj_cooling_temperature,LOX_inj_temperature,chamber_temperature,"
            "gps_year,gps_month,gps_day,gps_hour,gps_minute,gps_second,gps_centisecond,"
            "gps_lat,gps_lng,gps_alt,"
            "speed_x,speed_y,speed_z,"
            "accel_x,accel_y,accel_z,"
            "attitude_x,attitude_y,attitude_z,"
            "course,altitude,"
            "baro_pressure,baro_temperature,"
            "dpr_ok,ignited,calibrated,seperated,chute_opened,chute_unreefed,"
            "ignition_failed,catastrophic_failure,"
            "valve1,valve2,vent3,vent4,"
            "av_state");

    //check if the values are written to the file
    std::getline(file, line);
    //all the values are 0
    assert(line == "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
    //clear all lines in the file without deleting the file
    std::ofstream ofs;
    ofs.open(logFilePath, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    file.close();
    
    return 0;
}
