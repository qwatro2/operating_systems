#include <cstdlib>
#include <cstring>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "../common/arguments.h"
#include "../common/functions.h"

static volatile int keepRunning = 1;

void intHandler(int) {
    std::cout << "\033[32mSIGINT\033[0m\n";
    keepRunning = 0;
}

int main(int argc, const char *argv[])
{
    signal(SIGINT, intHandler);
    int client_socket;
    struct sockaddr_in server_addr;
    int recv_msg_size;

    RunMonitorArgs runMonitorArgs;
    if (!ParseArgs(runMonitorArgs, argc, argv)) {
        PrintHelpMonitor(argv[0]);
    }

    if ((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ExitWithError("\033[31msocket() failed\033[0m");
    }

    memset(&server_addr, 0, sizeof(server_addr));  
    server_addr.sin_family = AF_INET;        
    server_addr.sin_addr.s_addr = inet_addr(runMonitorArgs.server); 
    server_addr.sin_port = htons(runMonitorArgs.port);

    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        ExitWithError("\033[31mconnect() failed\033[0m");
    }

    auto sectors = new int[runMonitorArgs.numberOfSectors];
    std::cout << "\033[33mCurrent sectors' states, where 0 - empty, 1 - discovered, 2 - containts treasure\033[0m\n";

    int sendData[1];
    sendData[0] = 0;
    while (keepRunning) {
        recv(client_socket, sectors, sizeof(int) * runMonitorArgs.numberOfSectors, 0);

        std::cout << "\033[33m";
        for (auto i = 0; i < runMonitorArgs.numberOfSectors; ++i) {
            std::cout << sectors[i] << ' ';
        }
        std::cout << "\033[0m\n";
        send(client_socket, sendData, sizeof(sendData), 0);
        sleep(1);
    }

    recv(client_socket, sectors, sizeof(int) * runMonitorArgs.numberOfSectors, 0);
    sendData[0] = -1;
    send(client_socket, sendData, sizeof(sendData), 0);
    free(sectors);
    close(client_socket);
    return 0;
}
