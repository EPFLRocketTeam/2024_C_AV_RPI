
#include "data.h"
#include "logger.h"
#include <cassert>

#define LOG_FILE "log.txt"

int main (void) {
    DataDump dump;
    DataLogger logger(LOG_FILE);
    logger.conv(dump);

    FILE* file = fopen( LOG_FILE, "rb" );

    const int SIZE = sizeof(DataDump) + 1;
    char buffer[SIZE];

    size_t read_size = fread(buffer, 1, SIZE, file);
    fclose(file);

    assert(read_size == SIZE - 1);

    for (int i = 0; i + 1 < SIZE; i ++)
        assert(buffer[i] == 0);
}
