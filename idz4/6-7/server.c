#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

int port;
int total_sectors;
int treasure_sector;
int current_sector = 1;
bool server_running = true;
bool *sectors_checked;
pthread_mutex_t mutex;

struct sockaddr_in monitor_addr;
socklen_t monitor_len = 0;
bool monitor_active = false;

void handle_sigint(int sig) {
    server_running = false;
    puts("\nServer stopped\n");
    exit(0);
}

void *server_handler(void *arg) {
    int sock = *(int *)arg;
    struct sockaddr_in client;
    socklen_t cli_len = sizeof(client);
    char buffer[BUFFER_SIZE];
    int read_size;

    while (server_running) {
        if ((read_size = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, &cli_len)) > 0) {
            buffer[read_size] = '\0';

            pthread_mutex_lock(&mutex);
            char response_message[BUFFER_SIZE] = "Unknown command";

            if (strcmp(buffer, "register_monitor") == 0) {
                monitor_addr = client;
                monitor_len = cli_len;
                monitor_active = true;
                strcpy(response_message, "Monitor registered successfully");
            } else if (strcmp(buffer, "request_sector") == 0) {
                if (current_sector <= total_sectors && !sectors_checked[current_sector]) {
                    sectors_checked[current_sector] = true;
                    snprintf(response_message, sizeof(response_message), "%d", current_sector);
                    if (current_sector == treasure_sector) {
                        strcat(response_message, " Treasure found!");
                    }
                    current_sector++;
                } else {
                    strcpy(response_message, "All sectors checked");
                }
            }

            sendto(sock, response_message, strlen(response_message), 0, (struct sockaddr *)&client, cli_len);

            if (monitor_active) {
                char monitor_message[256];
                snprintf(monitor_message, sizeof(monitor_message), "Client request: %s\nServer response: %s\nCurrent sector: %d\n", buffer, response_message, current_sector - 1);
                sendto(sock, monitor_message, strlen(monitor_message), 0, (struct sockaddr *)&monitor_addr, monitor_len);
            }
            pthread_mutex_unlock(&mutex);
        }
    }
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    int socket_desc;
    struct sockaddr_in server;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <port> <total_sectors> <treasure_sector>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);
    total_sectors = atoi(argv[2]);
    treasure_sector = atoi(argv[3]);
    sectors_checked = calloc(total_sectors + 1, sizeof(bool));

    pthread_mutex_init(&mutex, NULL);
    signal(SIGINT, handle_sigint);

    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_desc == -1) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    puts("Server is running...");

    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, server_handler, (void *)&socket_desc) < 0) {
        perror("Could not create server thread");
        return 1;
    }

    pthread_join(server_thread, NULL);
    pthread_mutex_destroy(&mutex);
    free(sectors_checked);
    puts("Server stopped");
    return 0;
}
