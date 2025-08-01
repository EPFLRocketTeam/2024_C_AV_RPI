#include <iostream>
#include <unistd.h>
#include "data.h"
#include "sensors.h"
#include "telecom.h"

int main() {
    // Create drivers
    Sensors sensors;
    Telecom telecom;

    telecom.begin();
    telecom.send_telemetry();

    //sensors.update();

    usleep(500000);

    telecom.send_telemetry();

    //sensors.calibrate();
    //sensors.update();

    usleep(500000);

    telecom.send_telemetry();
}
