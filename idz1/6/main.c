#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

#include "../common/mylib.h"

const int BUFF_SIZE = 5000;

int main(const int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input> <output>", argv[0]);
        exit(1);
    }

    char input_buffer[BUFF_SIZE];
    char output_buffer[BUFF_SIZE];

    int fd1[2], fd2[2];
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        exit(1);
    }

    if (pid == 0) {
        close(fd1[0]);
        close(fd2[1]);

        int input_file = open(argv[1], O_RDONLY);

        if (input_file == -1) {
            exit(1);
        }

        ssize_t bytes_read = read(input_file, input_buffer, BUFF_SIZE);
        
        if (bytes_read == -1) {
            exit(1);
        }

        ssize_t bytes_written = write(fd1[1], input_buffer, bytes_read);
        
        if (bytes_written == -1) {
            exit(1);
        }

        bytes_read = read(fd2[0], output_buffer, BUFF_SIZE);

        if (bytes_read == -1) {
            exit(1);
        }

        int output_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);

        if (output_file == -1) {
            exit(1);
        }

        bytes_written = write(output_file, output_buffer, strlen(output_buffer));

        if (bytes_written == -1) {
            exit(1);
        }

        close(input_file);
        close(output_file);
        exit(0);
    } else {
        close(fd1[1]);
        close(fd2[0]);

        ssize_t bytes_read = read(fd1[0], input_buffer, BUFF_SIZE);

        if (bytes_read == -1) {
            exit(1);
        }

        solution(input_buffer, output_buffer);

        ssize_t bytes_written = write(fd2[1], output_buffer, strlen(output_buffer));

        if (bytes_written == -1) {
            exit(1);
        }

        close(fd1[0]);
        close(fd2[1]);
        wait(NULL);
    }

    return 0;
}
