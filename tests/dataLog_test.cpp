//
// Created by marin on 08.02.2025.
//

#include <cassert>
#include "data.h"
#include "datalogger.h"
#include <string>

int main(int argc, char** argv) {
    std::string str = "./logs/log_test.csv";
    DataDump dataDump;
    Datalogger datalogger = Datalogger(str,dataDump);

    datalogger.conv(dataDump);
}
