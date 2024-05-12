#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath> // for sqrt
#include <tuple>

#include "../include/data/data.h"
#include "../include/flightControl/AvState.h"
#include "../include/flightControl/Kalman.h"
#include "../include/data/sensors.h"

bool isFloat(const std::string & str) {
    try {
        std::stof(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::tuple<Data, AvState>> parseCSV(std::string filename) {

    std::vector<std::tuple<Data, AvState>> data;

    std::ifstream input{filename.c_str()};
    if (!input.is_open()) {
        std::cerr << "Error : " << strerror(errno) << "\n" << std::endl;
        std::cerr << "Couldn't read file: " << filename << "\n" << std::endl;
        return data; 
    }

    std::cout << "No error while opening file!\n" << std::endl;

    std::string line;

    // Skip the first line (header)
    // time(s),posx(m),posy(m),posz(m),vx(m/s),vy(m/s),vz(m/s),accx(m/s^2),accy(m/s^2),accz(m/s^2),
    // rotx(rad/s),roty(rad/s),rotz(rad/s),pression(Pa),flight_phase
    if (!std::getline(input, line)) {
        std::cerr << "Error: Failed to read the header line" << "\n" << std::endl;
        return data; // Return an empty vector
    }

    int lineNumber = 0;
    while (std::getline(input, line) /*&& lineNumber < 60764*/) { // CONDITION ON LINE_NUMBER!
        ++lineNumber;
        std::cout << "Line: " << lineNumber << std::endl;

        std::istringstream ss(line);
        Data avData;
        AvState avState;

        //TODO: setting sensors status (adxl and temperature of bmp desactivated) (only use first sensor)
        avData.set_adxl_status(0);
        avData.set_adxl_aux_status(0);
        avData.set_bmi_accel_status(BMI08_ACCEL_DATA_READY_INT);
        avData.set_bmi_gyro_status(BMI08_GYRO_DATA_READY_INT); // gyro activated!
        avData.set_bmi_aux_accel_status(0);
        avData.set_bmi_aux_gyro_status(0);

        bmp3_status bs = {{0,0,0}, {BMP3_CMD_RDY, BMP3_DRDY_PRESS, 0}, {0,0,0},0};
        bmp3_status bs_aux = {{0,0,0}, {0,0,0}, {0,0,0},0};
        avData.set_bmp_status(bs); //TODO: a revoir ici
        avData.set_bmp_aux_status(bs_aux);

        // Read each comma-separated value
        std::string value;

        // time (s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.set_time(std::stof(value));
        } else {
            std::cerr << "Error: Invalid time value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        double x = 0;
        double y = 0;
        double z = 0;

        // posx (m)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            x = std::stof(value);
        } else {
            std::cerr << "Error: Invalid posx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // posy (m)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            y = std::stof(value);
        } else {
            std::cerr << "Error: Invalid posy value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // posz == altitude (m)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            z = std::stof(value);
        } else {
            std::cerr << "Error: Invalid posz value on line " << lineNumber << value << "\n" << std::endl;
            break;
        }

        avData.set_altitude(z);

        double vx = 0;
        double vy = 0;
        double vz = 0;

        // vx (m/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            vx = std::stof(value);
        } else {
            std::cerr << "Error: Invalid vx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // vy (m/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            vy = std::stof(value);
        } else {
            std::cerr << "Error: Invalid vy value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // vz (m/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            vz = std::stof(value);
        } else {
            std::cerr << "Error: Invalid vz value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        avData.set_velocity({x,y,z});

        double ax = 0;
        double ay = 0;
        double az = 0;

        // accx (m/s²)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            ax = std::stof(value);
        } else {
            std::cerr << "Error: Invalid accx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // accy (m/s²)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            ay = std::stof(value);
        } else {
            std::cerr << "Error: Invalid accy value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // accz (m/s²)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            az = std::stof(value);
        } else {
            std::cerr << "Error: Invalid accz value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        avData.set_bmi_accel({(float)ax, (float)ay, (float)az});

        double rotx = 0;
        double roty = 0;
        double rotz = 0;

        // rotx (rad/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            rotx = std::stof(value);
        } else {
            std::cerr << "Error: Invalid rotx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // roty (rad/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            roty = std::stof(value);
        } else {
            std::cerr << "Error: Invalid roty value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // rotz (rad/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            rotz = std::stof(value);
        } else {
            std::cerr << "Error: Invalid rotz value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        avData.set_bmi_gyro({(float)rotx, (float)roty, (float)rotz});

        // pressure
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.set_bmp({0, std::stof(value)});
        } else {
            std::cerr << "Error: Invalid pressure value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // flight phase
        if (std::getline(ss, value, ',') && isFloat(value)) {
            int flightPhase = std::stof(value);
            if (flightPhase == 1) {
                avState.setCurrentState(State::IDLE);
            } else if (flightPhase == 2 || flightPhase == 22) {
                avState.setCurrentState(State::ASCENT);
            } else if (flightPhase == 3 || flightPhase == 4) {
                avState.setCurrentState(State::DESCENT);
            } else {
                std::cerr << "No corresponding flight phase value on line " << lineNumber << "\n" << std::endl;
                break;
            }

        } else {
            std::cerr << "Error: Invalid flight phase value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        data.push_back(std::make_tuple(avData, avState));
    }

    input.close();
    return data;

}

int main(void) {

    // Load data from CSV file
    std::vector<std::tuple<Data, AvState>> testData = parseCSV("../tests/Test_AVTrainingData.csv");

    KalmanFilter kalmanFilter = KalmanFilter();

    // Open a new CSV file for writing
    std::ofstream outputFile("../tests/output_Kalman1_onesensor_withgyro_test.csv");
    // Write headers to the CSV file
    outputFile << "Time,Estimated Velocity x,Estimated Velocity y,Estimated Velocity z,Estimated Altitude\n";

    int counter = 0;
    // Iterate over data and update Kalman filter
    for (auto & [avData, avState] : testData) {
        ++counter;
        std::cout << "Writing line " << counter << std::endl;

        std::tuple<double, Vector3> estimatedAltitudeAndVelocity = kalmanFilter.UpdateAndGetAltitudeAndVelocity(avData, avState);
        double estimatedAltitude = std::get<0>(estimatedAltitudeAndVelocity);
        Vector3 estimatedVelocity = std::get<1>(estimatedAltitudeAndVelocity);

        outputFile << avData.get_time() << "," << estimatedVelocity.x << "," << estimatedVelocity.y << "," << estimatedVelocity.z << "," << estimatedAltitude << "\n";

    }
    outputFile.close();
    return 0;

}