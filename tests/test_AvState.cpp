//
// Created by marin on 13/04/2024.
//

// just connect to the server and send a message and print the response
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include "../include/flightControl/AvState.h"
#include "../include/data/fakeData.h"
#include "../include/data/thresholds.h"


#define PORT 12345

int main()
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    const char* request = "request";
    char buffer[1024] = {0};
    DataDump dump;

    FakeData data;
    Thresholds thresholds = Thresholds();
    AvState avState = AvState(thresholds);

    while (true)
    {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            std::cerr << "Socket creation error" << std::endl;
            return -1;
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            return -1;
        }
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        {
            std::cerr << "Connection Failed" << std::endl;
            return -1;
        }
        std::cout << "Sending request" << std::endl;
        send(sock, request, strlen(request), 0);
        std::cout << "Request sent" << std::endl;
        valread = read(sock, buffer, 1024);
        // print the response in blue
        std::cout << "\033[1;34m";

        data.update(buffer);
        dump = data.dump();

        avState.update(dump.sensors_data, dump.telecom_status, dump.calibrated);
        std::cout << "Telecom status: " << data.dump().sensors_data.altitude << std::endl;
        // reset the color
        std::cout << "\033[0m";
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for 0.1 second
    }

    close(sock); // Close the socket
    return 0;
}
