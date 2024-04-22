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
    if (!std::getline(input, line)) {
        std::cerr << "Error: Failed to read the header line" << "\n" << std::endl;
        return data; // Return an empty vector
    }

    int lineNumber = 0;
    while (std::getline(input, line) && lineNumber < /*10*/ 60764-1) { // CONDITION ON LINE_NUMBER!
        ++lineNumber;
        std::cout << "Line: " << lineNumber << std::endl;

        std::istringstream ss(line);
        AvData avData;
        AvState avState;

        // Read each comma-separated value
        std::string value;

        // altitude (in m)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.altitude = std::stof(value);
        } else {
            std::cerr << "Error: Invalid altitude value on line " << lineNumber << "\n" << std::endl;
            break;
        }
        
        // velocity (in m/s)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.velocity = std::stof(value);
        } else {
            std::cerr << "Error: Invalid velocity value on line " << lineNumber << "\n" << std::endl;
            break;
        }
        
        // time (convert from milliseconds to seconds and add base)
        if (std::getline(ss, value, ',') && isFloat(value)) {
            avData.time = (std::stof(value) + 2463759.0) / 1000.0;
            if (avData.time >= 2465.04 && avData.time <= 2926.1655/*7175.29*/) {
                avState.setCurrentState(State::ASCENT);
            } else {
                avState.setCurrentState(State::IDLE);
            }
        } else {
            std::cerr << "Error: Invalid time value on line " << lineNumber << "\n" << std::endl;
            break;
        }
        
        // acceleration (extract x, y, z values from the tuple and use only x) (in m/s²)
        size_t start = line.find("(");
        size_t end = line.find(")", start);
        std::string accStr = line.substr(start + 1, end - start - 1);
        std::istringstream accStream(accStr);
        char comma;
        float x, y, z;
        if (accStream >> x >> comma >> y >> comma >> z) {
            //avData.acceleration = std::sqrt(x * x + y * y + z * z);
            avData.acceleration = x ; // in m/s^2? //std::abs(x);
        } else {
            std::cerr << "Error: Invalid acceleration value on line " << lineNumber << std::endl;
            break;
        }

        std::string remaining = line.substr(end + 3);
        std::istringstream remainingStream(remaining);

        // pressure
        if (std::getline(remainingStream, value, ',') && isFloat(value)) {
            avData.pressure = std::stof(value);
            avData.pressure *= 100.0f; // convert from hectopascals to pascals
        } else {
            std::cerr << "Error: Invalid pressure value on line " << lineNumber << isFloat(value) << "\n" << std::endl;
            break;
        }

        data.push_back(std::make_tuple(avData, avState));
    }

    input.close();
    return data;

}

int main(void) {

    // Load data from CSV file
    std::vector<std::tuple<AvData, AvState>> testData = parseCSV("../tests/combined_data.csv");

    KalmanFilter kalmanFilter = KalmanFilter();

    // Open a new CSV file for writing
    std::ofstream outputFile("../tests/output_Kalman_test.csv");
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
