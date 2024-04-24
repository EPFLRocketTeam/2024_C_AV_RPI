//
// Created by marin on 13/04/2024.
//

// just connect to the server and send a message and print the response
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <chrono>
#include <thread>

#define PORT 12345
int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    const char* request = "request";
    char buffer[1024] = {0};

    

   

    while (true) {

        
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

     if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       
        std::cerr << "Connection Failed" << std::endl;
        return -1;

    }

       

        std::cout << "Sending request" << std::endl;
        
        send(sock, request, strlen(request), 0);
        std::cout << "Request sent" << std::endl;
        
        valread = read(sock, buffer, 1024);
        // print the response in blue
        std::cout << "\033[1;34m";
        std::cout << "Response: " << buffer  << std::endl;
        // reset the color
        std::cout << "\033[0m";
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for 1 second
    }

    close(sock); // Close the socket
    return 0;
}