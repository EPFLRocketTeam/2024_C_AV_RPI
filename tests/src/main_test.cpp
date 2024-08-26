#include <iostream>
#include "data.h"
#include "telecom.h"
#include "sensors.h"

int main() {
    // Create a Data object
    Data data;

    // Create a Telecom object (if needed)
    Sensors sensors(data);
    Telecom telecom(data);


    return 0;
}
