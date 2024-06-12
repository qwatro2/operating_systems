#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 1024

int monitor_running = 1;

void handle_sigint(int sig) {
    monitor_running = 0;
    puts("\nMonitoring stopped\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char server_reply[BUFFER_SIZE];
    socklen_t server_len = sizeof(server);

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

    puts("Connected to server for monitoring");

    char *register_message = "register_monitor";
    if (sendto(sock, register_message, strlen(register_message), 0, (struct sockaddr *)&server, server_len) < 0) {
        puts("Failed to register as monitor");
        close(sock);
        return 1;
    }

    int recv_len = recvfrom(sock, server_reply, BUFFER_SIZE, 0, (struct sockaddr *)&server, &server_len);
    if (recv_len < 0) {
        puts("Failed to receive registration confirmation");
        close(sock);
        return 1;
    }

    server_reply[recv_len] = '\0';
    puts("Registration confirmation:");
    puts(server_reply);

    while (monitor_running) {
        recv_len = recvfrom(sock, server_reply, BUFFER_SIZE, 0, (struct sockaddr *)&server, &server_len);
        if (recv_len < 0) {
            puts("Receive failed");
            break;
        }

        server_reply[recv_len] = '\0';
        puts("\nServer update:");
        puts(server_reply);
    }

    close(sock);
    puts("Monitoring stopped");
    return 0;
}
