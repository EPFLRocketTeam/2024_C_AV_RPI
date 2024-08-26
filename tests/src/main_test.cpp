#include <iostream>
#include "data.h"
#include "telecom.h"

int main() {
    // Create a Data object
    Data data;

    // Create a Telecom object (if needed)
    Telecom telecom(data);

    // Update the data
    bool success = data.update();
    if (success) {
        // Data updated successfully, you can now access the data using the dump() method
        DataDump dump = data.dump();

        // Use the data as needed
        // ...

        // Send the data using the Telecom object (if needed)
        data.send(telecom);
    } else {
        // Failed to update data
        // Handle the error as needed
    }

    return 0;
}
