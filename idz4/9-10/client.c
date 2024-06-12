#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int sock;
int client_running = 1;
struct sockaddr_in server;
socklen_t server_len;

void handle_sigint(int sig) {
    client_running = 0;
    puts("\nClient stopped\n");
    close(sock);
    exit(0);
}

void *check_shutdown(void *arg) {
    char buffer[BUFFER_SIZE];
    while (client_running) {
        int recv_len = recvfrom(sock, buffer, BUFFER_SIZE, MSG_DONTWAIT, (struct sockaddr *)&server, &server_len);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            if (strstr(buffer, "Server shutting down") != NULL) {
                puts("Server shutting down, client stopping...");
                client_running = 0;
                break;
            }
        }
        usleep(100000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    char server_reply[BUFFER_SIZE];
    pthread_t shutdown_thread;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    signal(SIGINT, handle_sigint);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        printf("Socket creation failed");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server_len = sizeof(server);

    puts("Connected to server");

    if (pthread_create(&shutdown_thread, NULL, check_shutdown, NULL) != 0) {
        perror("Could not create shutdown checking thread");
        return 1;
    }

    while (client_running) {
        char *message = "request_sector";
        if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&server, server_len) < 0) {
            puts("Send failed");
            break;
        }

        int recv_len = recvfrom(sock, server_reply, BUFFER_SIZE, 0, (struct sockaddr *)&server, &server_len);
        if (recv_len < 0) {
            puts("Receive failed");
            break;
        }

        server_reply[recv_len] = '\0';
        if (strstr(server_reply, "All sectors checked") != NULL) {
            puts(server_reply);
            break;
        }

        int sector = atoi(server_reply);
        printf("Checking sector %d...\n", sector);

        if (strstr(server_reply, "Treasure found!") != NULL) {
            printf("Treasure found at sector %d!\n", sector);
            break;
        } else {
            printf("No treasure at sector %d.\n", sector);
        }

        sleep(1);
    }

    client_running = 0;
    pthread_join(shutdown_thread, NULL);
    close(sock);
    puts("Client stopped");
    return 0;
}
