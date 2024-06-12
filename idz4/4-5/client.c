#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    char* server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Cannot create socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    while (1) {
        strcpy(buffer, "request_sector");
        sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        int recv_len = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            if (strcmp(buffer, "All sectors checked") == 0) {
                printf("%s\n", buffer);
                break;
            }

            int sector = atoi(buffer);
            printf("Checking sector %d...\n", sector);
            sleep(1);

            sprintf(buffer, "%d", sector);
            sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

            recv_len = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
            if (recv_len > 0) {
                buffer[recv_len] = '\0';
                if (strcmp(buffer, "Treasure found!") == 0) {
                    printf("Treasure found at sector %d!\n", sector);
                    break;
                } else {
                    printf("No treasure at sector %d.\n", sector);
                }
            }
        }
    }

    close(socket_fd);
    return 0;
}
