#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath> // for sqrt
#include <tuple>

#include "../include/flightControl/AvData.h"
#include "../include/flightControl/AvState.h"
#include "../include/flightControl/Kalman.h"

bool isFloat(const std::string & str) {
    try {
        std::stof(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::tuple<AvData, AvState>> parseCSV(std::string filename) {

    std::vector<std::tuple<AvData, AvState>> data;

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
        AvData avData;
        AvState avState;

        // Read each comma-separated value
        std::string value;

        // time (s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.time = std::stof(value);
        } else {
            std::cerr << "Error: Invalid time value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // posx (m)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid posx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // posy (m)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid posy value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // posz == altitude (m)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.altitude = std::stof(value);
        } else {
            std::cerr << "Error: Invalid posz value on line " << lineNumber << value << "\n" << std::endl;
            break;
        }

        // vx (m/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid vx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // vy (m/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid vy value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // vz (m/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.velocity = std::stof(value);
        } else {
            std::cerr << "Error: Invalid vz value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // accx (m/s²)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid accx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // accy (m/s²)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid accy value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // accz (m/s²)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.acceleration = std::stof(value);
        } else {
            std::cerr << "Error: Invalid accz value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // rotx (rad/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid rotx value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // roty (rad/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid roty value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // rotz (rad/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            // nothing
        } else {
            std::cerr << "Error: Invalid rotz value on line " << lineNumber << "\n" << std::endl;
            break;
        }

        // pressure
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.pressure = std::stof(value);
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
    std::vector<std::tuple<AvData, AvState>> testData = parseCSV("../tests/Test_AVTrainingData.csv");

    KalmanFilter kalmanFilter = KalmanFilter();

    // Open a new CSV file for writing
    std::ofstream outputFile("../tests/new_output_Kalman_test.csv");
    // Write headers to the CSV file
    outputFile << "Time,Estimated Velocity,Estimated Altitude\n";

    int counter = 0;
    // Iterate over data and update Kalman filter
    for (const auto & [avData, avState] : testData) {
        ++counter;
        std::cout << "Writing line " << counter << std::endl;

        std::tuple<float, float> estimatedAltitudeAndVelocity = kalmanFilter.UpdateAndGetAltitudeAndVelocity(avData, avState);
        float estimatedAltitude = std::get<0>(estimatedAltitudeAndVelocity);
        float estimatedVelocity = std::get<1>(estimatedAltitudeAndVelocity);

        outputFile << avData.time << "," << estimatedVelocity << "," << estimatedAltitude << "\n";

    }
    outputFile.close();
    return 0;

}
