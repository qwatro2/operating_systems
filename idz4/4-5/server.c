#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <port> <total_sectors> <treasure_sector>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int total_sectors = atoi(argv[2]);
    int treasure_sector = atoi(argv[3]);
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    bool sectors_checked[total_sectors + 1];
    memset(sectors_checked, false, sizeof(sectors_checked));
    int current_sector = 1;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Cannot create socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(socket_fd);
        return 1;
    }

    printf("Server is running on port %d, managing %d sectors, treasure at sector %d\n", port, total_sectors, treasure_sector);

    while (1) {
        int recv_len = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            if (strcmp(buffer, "request_sector") == 0) {
                while (current_sector <= total_sectors && sectors_checked[current_sector]) {
                    current_sector++;
                }
                if (current_sector > total_sectors) {
                    strcpy(buffer, "All sectors checked");
                } else {
                    sectors_checked[current_sector] = true;
                    sprintf(buffer, "%d", current_sector);
                }
            } else if (atoi(buffer) == treasure_sector) {
                strcpy(buffer, "Treasure found!");
                break;
            } else {
                strcpy(buffer, "No treasure.");
            }
            sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len);
        }
    }

    close(socket_fd);
    return 0;
}
