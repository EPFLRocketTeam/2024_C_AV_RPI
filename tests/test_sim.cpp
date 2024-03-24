#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "../include/data/fakeData.h"
#include "../include/flightControl/AvState.h"

int main() {

    #include <fstream>

    FakeData data;
    AvState state = AvState(data);
    //create a csv file to store the data
    FILE* file = fopen("data.csv", "w");
    fprintf(file, "Time, State\n");

    std::cout << "Client started" << std::endl;
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        return 1;
    }
    std::cout << "Client socket created" << std::endl;

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    std::cout << "Server address created" << std::endl;

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connect");
        return 1;
    }
    std::cout << "Connected to server" << std::endl;

    while (true) {
        std::cout << "Sending request" << std::endl;
        const char *msg = "request\n";
        send(client_socket, msg, strlen(msg), 0);

        char buffer[1024] = {0};
        std::cout << "Waiting for response" << std::endl;
        std::cout << client_socket << std::endl;
        recv(client_socket, buffer, 1024, 0);
        
        std::cout << "Received: " << buffer << std::endl;

        data.set_clean_data(buffer);
        state.update(data);
        
        fprintf(file, "%d, %s\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), state.stateToString(state.getCurrentState()).c_str());

        std::cout << "Current state: " << state.stateToString(state.getCurrentState()) << std::endl;





        std::this_thread::sleep_for(std::chrono::microseconds(100000));
    }

    close(client_socket);
    return 0;
}
