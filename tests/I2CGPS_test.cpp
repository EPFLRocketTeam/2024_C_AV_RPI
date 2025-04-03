// Test code for SparkFun GPS Breakout - XA1110 - Alexis Limozin

#include <iostream>
#include <iomanip>
#include "TinyGPS++.h"
#include "I2CGPS.h"
#include <fstream>

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT8_C(3)

static void displayInfo(TinyGPSPlus& gps) {
    
    //create log file
    std::ofstream log("/home/av/logd/gps_test1.log", std::ios::app);
    log << "Calibrating..." << std::endl;
    log << "GPS test" << std::endl;
    log << std::endl << "GPS DATA" << std::endl
        << "GPS data in m/s^2" << std::endl
        << "Sample_Count, GPS_Raw_Lat, GPS_Raw_Lng, GPS_ms2_Lat, GPS_ms2_Lng\n";

    //We have new GPS data to deal with!
    if (gps.time.isValid()) {
        log << "Date: "
                  << (int)gps.date.day() << "/"
                  << (int)gps.date.month()  << "/"
                  << (int)gps.date.year();

        log << " Time: ";
        if (gps.time.hour() < 10) log << '0';
        log << (int)gps.time.hour() << ":";

        if (gps.time.minute() < 10) log << '0';
        log << (int)gps.time.minute() << ":";

        if (gps.time.second() < 10) log << '0';
        log << (int)gps.time.second() << ":";

        if (gps.time.centisecond() < 10) log << '0';
        log << (int)gps.time.centisecond() << std::endl; // Done printing time
    } else {
        log << "Time not yet valid" << std::endl;
    }

    if (gps.location.isValid()) {
        log << "Location: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << "lat: " << gps.location.lat() << " deg, lng : "
                  << gps.location.lng() << " deg." << std::endl;
    } else {
        log << "Location not yet valid" << std::endl;
    }

    if (gps.altitude.isValid()) {
        log << "Altitude: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << gps.altitude.meters() << " m." << std::endl; //do .kilometers() to get km
    } else {
        log << "Altitude not yet valid" << std::endl;
    }
    
    if (gps.course.isValid()) {
        log << "Course: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << gps.course.deg() << " deg." << std::endl;
    } else {
        log << "Course not yet valid" << std::endl;
    }

    if (gps.speed.isValid()) {
        log << "Speed: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << gps.speed.mps() << " m/s." << std::endl; //in m/s, do .kmph() for km/h
    } else {
        log << "Speed not yet valid" << std::endl;
    }
}

int main(void) {
    std::ofstream log("/home/av/logd/gps_test2.log", std::ios::app);
    log << "init" << std::endl;
    try{
    I2CGPS i2cgps;
    TinyGPSPlus gps;
    int loop = 0;
    while (loop < ITERATION) {
        while (i2cgps.available()) {
            //For debugging, just output the raw NMEA sentences
            // uint8_t incoming = i2cgps.read();
            // if (incoming == '$') log << std::endl;
            // log << incoming;
            gps.encode(i2cgps.read());
        }
        if (gps.time.isUpdated()) {
            displayInfo(gps);
        }
        loop++;
    }

}
catch(const std::exception& e)
{
    log << e.what() << '\n';
    log.close();
}
log.close();
}

