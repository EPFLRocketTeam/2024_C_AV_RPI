
#include <bits/stdc++.h>

bool readEvent (std::ifstream &file) {
    uint32_t ts, str_length;
    printf("Reading event...\n");
    file.read(reinterpret_cast<char*>(&ts), sizeof(uint32_t));
    printf("Timestamp: %lld\n", (long long) ts);
    if (file.eof()) return false;
    file.read(reinterpret_cast<char*>(&str_length), sizeof(uint32_t));

    char* buffer = (char*) malloc((str_length + 1) * sizeof(char));
    buffer[str_length] = 0;

    file.read(buffer, str_length * sizeof(char));

    printf("[%lld] %s\n", (long long) ts, buffer);

    free(buffer);

    return true;
}

int main (int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage event2logs <file>");
        return 1;
    }

    std::ifstream file;
    file.open(argv[1]);

    bool shouldContinue;
    do {
        shouldContinue = readEvent(file);
    } while (shouldContinue);

    file.close();
}
