
#include  "../include/flightControl/AvData.h"
#include <iostream>

// Define global variables here

// Implement functions here

void AvData::update(std::string telemetry){
    // Parse the telemetry string and update the data members
    std::string delimiter = ", ";
    size_t pos = 0;
    std::string token;
    
    while ((pos = telemetry.find(delimiter)) != std::string::npos) {
        token = telemetry.substr(0, pos);
        size_t colonPos = token.find(": ");
        std::string key = token.substr(0, colonPos);
        std::string value = token.substr(colonPos + 2);
        
        // Update the corresponding data member based on the key
        if (key == "pressure") {
            // Update pressure data member with value
        } else if (key == "temperature") {
            // Update temperature data member with value
        } else if (key == "altitude") {
            // Update altitude data member with value
        } else if (key == "acceleration") {
            // Update acceleration data member with value
        } else if (key == "velocity") {
            // Update velocity data member with value
        } else if (key == "thrust") {
            // Update thrust data member with value
        } else if (key == "battery") {
            // Update battery data member with value
        } else if (key == "voltage") {
            // Update voltage data member with value
        } else if (key == "current") {
            // Update current data member with value
        } else if (key == "power") {
            // Update power data member with value
        } else if (key == "energy") {
            // Update energy data member with value
        } else if (key == "capacity") {
            // Update capacity data member with value
        } else if (key == "efficiency") {
            // Update efficiency data member with value
        } else if (key == "time") {
            // Update time data member with value
        } else if (key == "ignited") {
            // Update ignited data member with value
        }
        
        telemetry.erase(0, pos + delimiter.length());
    }

    // Parse the telemetry string and update the data members
    // Example: "pressure: 1000.0, temperature: 25.0, altitude: 100.0, acceleration: 9.8, velocity: 0.0, thrust: 0.0, battery: 100.0, voltage: 12.0, current: 0.0, power: 0.0, energy: 0.0, capacity: 100.0, efficiency: 0.0, time: 0.0, ignited: false"


    }


