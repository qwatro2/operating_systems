#include <cstdlib>
#include <cstring>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "../common/arguments.h"
#include "../common/functions.h"

using Mutex = pthread_mutex_t;
using ThreadArgsPtr = ThreadArgs*;

Mutex mutex;
int* sectors;
auto found = 0;
auto printed = 0;

void *listenThread(void* args) {
    int socket;
    int client_length;
    int client_socket;
    struct sockaddr_in client_addr;

    pthread_detach(pthread_self());
    socket = ((ThreadArgsPtr)args)->socket;
    int sectorsCount = ((ThreadArgsPtr)args)->numberOfSectors;
    free(args);

    listen(socket, 8);

    while (true) {
        client_length = sizeof(client_addr);
        client_socket = accept(socket, (struct sockaddr *) &client_addr, (socklen_t*) &client_length);
        std::cout << "\033[32mNew connection from " << inet_ntoa(client_addr.sin_addr) << "\033[0m\n";

        while (true) {
            if (found) {
                break;
            }

            send(client_socket, sectors, sizeof(int) * sectorsCount, 0);
            sleep(1);
        }
        close(client_socket);
    }
}

void* clientThread(void* args) {
    pthread_detach(pthread_self());
    auto socket = ((ThreadArgsPtr)args)->socket;
    auto sectorsCount = ((ThreadArgsPtr)args)->numberOfSectors;
    free(args);

    SendData sendData;
    RecievedData recievedData;

    int hasTreasure = 0;
    int selectedSector = -1;

    while (true) {
        pthread_mutex_lock(&mutex);
        for (auto i = 0; i < sectorsCount; ++i) {
            if (sectors[i] != 1) {
                selectedSector = i;
		        hasTreasure = sectors[i] == 2;
		        sectors[i] = 1;
                break;
            }
        }
	    pthread_mutex_unlock(&mutex);

	    sendData.selectedSector = selectedSector;
	    sendData.hasTreasure = hasTreasure;
        
        send(socket, &sendData, sizeof(sendData), 0);
    
	    if (selectedSector == -1) {
            break;
        }

        recv(socket, &recievedData, sizeof(recievedData), 0);

        pthread_mutex_lock(&mutex);
        if (recievedData.treasureFound) {
            if (!printed) {
                std::cout << "\033[33mPirates found treasure!\033[0m\n";
                printed = 1;
            }

	        for (int i = 0; i < sectorsCount; i++) {
	       	  sectors[i] = 1;
	        }
        }
        pthread_mutex_unlock(&mutex);
    
	    selectedSector = -1;
    }
    close(socket);
}

int main(int argc, const char *argv[]) {
    int server_socket;
    int client_socket;
    int client_length;

    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct sockaddr_in server_listen_addr;

    int listen_server_socket;

    pthread_t threadId;
    pthread_mutex_init(&mutex, NULL);
    
    RunServerWithMonitorArgs runServerWithMonitorArgs;
    if (!ParseArgs(runServerWithMonitorArgs, argc, argv)) {
        PrintHelpServerWithMonitor(argv[0]);
        return 0;
    }

    sectors = new int[runServerWithMonitorArgs.numberOfSectors];
    sectors[runServerWithMonitorArgs.treasureIndex] = 2;

    if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ExitWithError("\033[31msocket() failed\033[0m");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;              
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(runServerWithMonitorArgs.port);

    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        ExitWithError("\033[31mbind() failed\033[0m");
    }

    std::cout << "\033[32mOpen socket on " << inet_ntoa(server_addr.sin_addr) << ": " << runServerWithMonitorArgs.port << "\033[0m\n";
    
    if ((listen_server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ExitWithError("\033[31msocket() failed\033[0m");
    }

    memset(&server_listen_addr, 0, sizeof(server_listen_addr));
    server_listen_addr.sin_family = AF_INET;              
    server_listen_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_listen_addr.sin_port = htons(runServerWithMonitorArgs.monitorPort);

    if (bind(listen_server_socket, (struct sockaddr *) &server_listen_addr, sizeof(server_listen_addr)) < 0) {
        ExitWithError("\033[31mbind() failed\033[0m");
    }

    std::cout << "\033[32mOpen socket on " << inet_ntoa(server_listen_addr.sin_addr) << ": " << runServerWithMonitorArgs.monitorPort << "\033[0m\n";

    auto args2 = new ThreadArgs;
    args2->socket = listen_server_socket;
    args2->numberOfSectors = runServerWithMonitorArgs.numberOfSectors;
    if (pthread_create(&threadId, NULL, listenThread, (void*) args2) != 0) {
        ExitWithError("\033[31mpthread_create() failed\033[0m");
    }

    listen(server_socket, 8);
    while (true) {
        client_length = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*) &client_addr, (socklen_t*) &client_length);
        std::cout << "\033[32mNew connection from " << inet_ntoa(client_addr.sin_addr) << "\033[0m\n";

        auto args = new ThreadArgs;
        args->socket = client_socket;
        args->numberOfSectors = runServerWithMonitorArgs.numberOfSectors;
        if (pthread_create(&threadId, NULL, clientThread, (void*) args) != 0) {
            ExitWithError("\033[31mpthread_create() failed\033[0m");
        }
    }

    delete[] sectors;
    pthread_mutex_destroy(&mutex);
    return 0;
}
