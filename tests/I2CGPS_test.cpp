// Test code for SparkFun GPS Breakout - XA1110 - Alexis Limozin

#include <iostream>
#include <iomanip>
#include "TinyGPS++.h"
#include "I2CGPS.h"
#include "av_timer.h"

/************************************************************************/
/*********                     Test code                           ******/
/************************************************************************/
/* Iteration count to run example code */
#define ITERATION  UINT8_C(100)

static void displayInfo(TinyGPSPlus& gps) {
    //We have new GPS data to deal with!
    if (gps.time.isValid()) {
        std::cout << "Date: "
                  << (int)gps.date.day() << "/"
                  << (int)gps.date.month()  << "/"
                  << (int)gps.date.year();

        std::cout << " Time: ";
        if (gps.time.hour() < 10) std::cout << '0';
        std::cout << (int)gps.time.hour() << ":";

        if (gps.time.minute() < 10) std::cout << '0';
        std::cout << (int)gps.time.minute() << ":";

        if (gps.time.second() < 10) std::cout << '0';
        std::cout << (int)gps.time.second() << ":";

        if (gps.time.centisecond() < 10) std::cout << '0';
        std::cout << (int)gps.time.centisecond() << std::endl; // Done printing time
    } else {
        std::cout << "Time not yet valid" << std::endl;
    }

    if (gps.location.isValid()) {
        std::cout << "Location: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << "lat: " << gps.location.lat() << " deg, lng : "
                  << gps.location.lng() << " deg." << std::endl;
    } else {
        std::cout << "Location not yet valid" << std::endl;
    }

    if (gps.altitude.isValid()) {
        std::cout << "Altitude: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << gps.altitude.meters() << " m." << std::endl; //do .kilometers() to get km
    } else {
        std::cout << "Altitude not yet valid" << std::endl;
    }
    
    if (gps.course.isValid()) {
        std::cout << "Course: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << gps.course.deg() << " deg." << std::endl;
    } else {
        std::cout << "Course not yet valid" << std::endl;
    }

    if (gps.speed.isValid()) {
        std::cout << "Speed: "
                  << std::fixed << std::setprecision(6) // Set precision
                  << gps.speed.mps() << " m/s." << std::endl; //in m/s, do .kmph() for km/h
    } else {
        std::cout << "Speed not yet valid" << std::endl;
    }
    std::cout << "\n";
}

int main(void) {
    I2CGPS i2cgps;
    TinyGPSPlus gps;
    int loop = 0;
    while (loop < ITERATION) {
        while (i2cgps.available()) {
            //For debugging, just output the raw NMEA sentences
            // uint8_t incoming = i2cgps.read();
            // if (incoming == '$') std::cout << std::endl;
            // std::cout << incoming;
            gps.encode(i2cgps.read());
        }
        if (gps.time.isUpdated()) {
            displayInfo(gps);
        }
        loop++;
        AvTimer::sleep(10);
    }
}

