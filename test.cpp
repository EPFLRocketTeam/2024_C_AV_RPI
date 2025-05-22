
#include <format>
#include <string>
#include <iostream>
#include <stdarg.h>

template<typename... Args>
std::string myfmt2 (const char* fmt, Args&&... args) {
    int size = std::snprintf(NULL, 0, fmt, args...);
    if (size < 0) {
        throw std::exception();
    }

    char* buffer = (char*) malloc( (size + 1) * sizeof(char) );

    std::snprintf(buffer, size + 1, fmt, args...);

    std::string result(size, '.');
    for (size_t off = 0; off < size; off ++)
        result[off] = buffer[off];

    free(buffer);

    return buffer;
}

std::string myfmt (const char* fmt, ...) {
    va_list list;
    va_start(list, fmt);

    va_list list_copy;
    va_copy(list_copy, list);
    int size = std::vsnprintf(NULL, 0, fmt, list_copy);
    va_end(list_copy);

    if (size <= 0) throw std::exception();

    char* buffer = (char*) malloc( (size + 1) * sizeof(char) );

    std::vsnprintf(buffer, size + 1, fmt, list);

    std::string result(size, '.');
    for (size_t off = 0; off < size; off ++)
        result[off] = buffer[off];

    free(buffer);
    va_end(list);

    return result;
}

int main () {
    std::cout << myfmt("Some format %d : %s\n", 1, "Hello, World !");
    std::cout << myfmt2("Some format %d : %s\n", 2, "Hell, World !");
}
