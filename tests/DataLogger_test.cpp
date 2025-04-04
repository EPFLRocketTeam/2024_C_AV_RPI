
#include "data.h"
#include "logger.h"
#include <cassert>
#include <iostream>

#define LOG_FILE "/tmp/log.txt"

int main (void) {
    DataDump dump;
    {
        DataLogger::getInstance(LOG_FILE, "event_log.txt").conv(dump);
        
    }

    char* expects = (char*) (&dump);
    //check if the file exists if not create it

    FILE* checkFile = fopen(LOG_FILE, "rb");
    if (!checkFile) {
        checkFile = fopen(LOG_FILE, "wb");
        assert(checkFile != nullptr); // Ensure the file was created successfully
        fclose(checkFile);
    } else {
        fclose(checkFile);
    }
    FILE* file = fopen( LOG_FILE, "rb" );

    const int SIZE = sizeof(DataDump) + 1;
    char buffer[SIZE];
    memset(buffer, 0, SIZE);

    size_t read_size = fread(buffer, 1, SIZE, file);
    fclose(file);
std::cout << "Read size: " << read_size << std::endl;
    assert(read_size == SIZE - 1);

    for (int i = 0; i + 1 < SIZE; i ++)
        assert(buffer[i] == expects[i]);
}
