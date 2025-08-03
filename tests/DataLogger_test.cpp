
#include "data.h"
#include "logger.h"
#include <cassert>

#define LOG_FILE "./log.txt"
#define EVT_FILE "./event.txt"

int main (void) {
    DataDump dump;
    std::string path;

    assert(DataLogger::init(LOG_FILE, EVT_FILE));
    path = DataLogger::get_dump_path();
    DataLogger::log_dump(dump);

    assert (path.substr(0, 6) == "./log_");
    assert (path.substr(path.size() - 4) == ".txt");
    DataLogger::terminate();

    char* expects = (char*) (&dump);

    FILE* file = fopen( path.c_str(), "rb" );

    const int SIZE = sizeof(DataDump) + 1;
    char buffer[SIZE];

    size_t read_size = fread(buffer, 1, SIZE, file);
    fclose(file);

    assert(read_size == SIZE - 1);

    for (int i = 0; i + 1 < SIZE; i ++)
        assert(buffer[i] == expects[i]);
}
