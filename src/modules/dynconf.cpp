#include <iostream>
#include <fstream>
#include <locale>
#include "dynconf.h"
#include "logger.h"

std::string trim(std::string buffer) {
    int left  = 0;
    int right = buffer.size() - 1;
    while (left <= right && std::isspace(buffer[left])) left++;
    while (left <= right && std::isspace(buffer[right])) right--;

    if (right < left) return "";
    return buffer.substr(left, right - left + 1);
}

void ConfigManager::init(std::string path) {
    std::ifstream file(path.c_str());

    std::string line_buf;
    std::string section = "";
    while (std::getline(file, line_buf)) {
        if (file.fail()) {
            Logger::log_eventf(Logger::ERROR, "Configuration file \"%s\" not found", path.c_str());
            break;
        }

        line_buf = trim(line_buf);
        if (line_buf == "") continue;

        std::size_t pos = line_buf.find_first_of('=');

        if (line_buf.size() >= 2 && line_buf[0] == '[' && line_buf.back() == ']') {
            section = line_buf.substr(1, line_buf.size() - 2);
        } else if (pos != std::string::npos) {
            std::string lhs = trim(line_buf.substr(0, pos));
            std::string rhs = trim(line_buf.substr(pos + 1));

            std::string target = lhs;
            if (section.size() != 0) {
                target = section + "." + lhs;
            }
            
            if (rhs != "on" && rhs != "off") {
                std::cout << "ERROR: line '" << line_buf << "' could not be parsed." << std::endl;
                std::cout << " - The assignment value isn't 'on' or 'off'." << std::endl;
                continue;
            }

            content[target] = rhs == "on";
        } else {
            std::cout << "ERROR: line '" << line_buf << "' could not be parsed." << std::endl;
            std::cout << " - The line isn't empty." << std::endl;
            std::cout << " - The line isn't of the section form '[section]'." << std::endl;
            std::cout << " - The line isn't an assignment 'a=b'." << std::endl;
            continue;
        }
    }
}

/*
int main () {
    ConfigManager::initConfig("example_config.conf");

    std::cout << "kalman        : " << ConfigManager::isEnabled("kalman")        << std::endl;
    std::cout << "sensors.adxl1 : " << ConfigManager::isEnabled("sensors.adxl1") << std::endl;
    std::cout << "sensors.adxl2 : " << ConfigManager::isEnabled("sensors.adxl2") << std::endl;
    std::cout << "sensors.bmi1  : " << ConfigManager::isEnabled("sensors.bmi1")  << std::endl;
    std::cout << "sensors.bmi2  : " << ConfigManager::isEnabled("sensors.bmi2")  << std::endl;
}

// The result of the following main function is
// 
// kalman        : 1
// sensors.adxl1 : 0
// sensors.adxl2 : 1
// sensors.bmi1  : 1
// sensors.bmi2  : 0
*/
