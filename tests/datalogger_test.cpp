
#include "data.h"
#include "logger.h"
#include <cassert>

#define LOG_FILE "./log.txt"
#define EVT_FILE "./event.txt"

int main (void) {
    DataDump dump;
    std::string path;

    assert(Logger::init(LOG_FILE, EVT_FILE));
    path = Logger::get_dump_path();
    Logger::log_dump(dump);

    assert (path.substr(0, 6) == "./log_");
    assert (path.substr(path.size() - 4) == ".txt");

    for (int i(0); i < Logger::NB_SEVERITY; ++i) {
        Logger::log_eventf((Logger::Severity)i, "Event log test message (%u)", dump.av_timestamp, i);
    }
    Logger::terminate();

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
