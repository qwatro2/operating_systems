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
using Sectors = std::vector<int>;
using ThreadArgsPtr = ThreadArgs*;

Mutex mutex;
Sectors sectors;
auto found = 0;
auto printed = 0;

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

    pthread_t threadId;
    pthread_mutex_init(&mutex, NULL);
    
    RunServerArgs runServerArgs;
    if (!ParseArgs(runServerArgs, argc, argv)) {
        PrintHelpServer(argv[0]);
        return 0;
    }

    sectors.resize(runServerArgs.numberOfSectors, 0);
    sectors[runServerArgs.treasureIndex] = 2;

    if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ExitWithError("\033[31msocket() failed\033[0m");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;              
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(runServerArgs.port);

    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        ExitWithError("\033[31mbind() failed\033[0m");
    }

    std::cout << "\033[32mOpen socket on " << inet_ntoa(server_addr.sin_addr) << ": " << runServerArgs.port << "\033[0m\n";
    listen(server_socket, 8);

    while(true) {
        client_length = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*) &client_addr, (socklen_t*) &client_length);
        std::cout << "\033[32mNew connection from " << inet_ntoa(client_addr.sin_addr) << "\033[0m\n";

        auto args = new ThreadArgs;
        args->socket = client_socket;
        args->numberOfSectors = runServerArgs.numberOfSectors;
        if (pthread_create(&threadId, NULL, clientThread, (void*) args) != 0) {
            ExitWithError("\033[31mpthread_create() failed\033[0m");
        }
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
