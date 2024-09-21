#include <iostream>
#include <unistd.h>
#include "data.h"
#include "telecom.h"
#include "sensors.h"

int main() {
    // Create drivers
    Sensors sensors;
    Telecom telecom;

    telecom.begin();
    telecom.send_telemetry();

    sensors.update();

    usleep(500000);

    telecom.send_telemetry();

    sensors.calibrate();
    sensors.update();

    usleep(500000);

    telecom.send_telemetry();
}
