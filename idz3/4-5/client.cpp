#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

#include "../common/arguments.h"
#include "../common/functions.h"

int main(int argc, const char *argv[]) {
    int client_socket;
    struct sockaddr_in server_addr;
    int recv_msg_size;

    RunClientArgs runClientArgs;
    if (!ParseArgs(runClientArgs, argc, argv)) {
        PrintHelpClient(argv[0]);
        return 0;
    }
    
    if ((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ExitWithError("\033[31msocket() failed\033[0m");
    }

    memset(&server_addr, 0, sizeof(server_addr));  
    server_addr.sin_family = AF_INET;        
    server_addr.sin_addr.s_addr = inet_addr(runClientArgs.server); 
    server_addr.sin_port = htons(runClientArgs.port);
    
    if (connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        ExitWithError("\033[31mconnect() failed\033[0m");
    }

    RecievedData recievedData;
    SendData sendData;

    while (true) {
        recv(client_socket, &sendData, sizeof(sendData), 0);
    
        if (sendData.selectedSector == -1) {
            break;
        }

        std::cout << "\033[33mPirates check sector " << sendData.selectedSector << "\033[0m\n";
        sleep(1 + rand() % 3);
    
        if (sendData.hasTreasure) {
            std::cout << "\033[33mPirates found treasure!\033[0m\n";
            recievedData.treasureFound = true;
        } else {
            recievedData.treasureFound = false;
        }

        send(client_socket, &recievedData, sizeof(recievedData), 0);
    }

    std::cout << "\033[33mPirates finished\033[0m\n";
    close(client_socket);
    return 0;
}
