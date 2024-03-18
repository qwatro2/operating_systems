#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "../common/mylib.h"

const int BUFFER_SIZE = 5000;

int main(const int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input> <output>", argv[0]);
        exit(1);
    }

    char input_buffer[BUFFER_SIZE];
    char output_buffer[BUFFER_SIZE];
    const char *fifo1 = "vovan";
    const char *fifo2 = "antoha";

    mkfifo(fifo1, 0666);
    mkfifo(fifo2, 0666);

    int input_file = open(argv[1], O_RDONLY);

    if (input_file == -1) {
        exit(1);
    }

    int output_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (output_file == -1) {
        exit(1);
    }

    pid_t pid1, pid2, pid3;

    if ((pid1 = fork()) == -1) {
        exit(1);
    }

    if (pid1 == 0) {
        int fifo1_fd = open(fifo1, O_WRONLY);

        if (fifo1_fd == -1) {
            exit(1);
        }

        ssize_t bytes_read = read(input_file, input_buffer, BUFFER_SIZE);

        if (bytes_read == -1) {
            exit(1);
        }

        write(fifo1_fd, input_buffer, bytes_read);
        close(fifo1_fd);
        exit(0);
    } else {
        if ((pid2 = fork()) == -1) {
            exit(1);
        }

        if (pid2 == 0) {
            int fifo1_fd = open(fifo1, O_RDONLY);
            if (fifo1_fd == -1) {
                exit(1);
            }

            int fifo2_fd = open(fifo2, O_WRONLY);
            if (fifo2_fd == -1) {
                exit(1);
            }

            ssize_t bytes_read = read(fifo1_fd, input_buffer, BUFFER_SIZE);
            if (bytes_read == -1) {
                exit(1);
            }

            solution(input_buffer, output_buffer);
            
            ssize_t bytes_written = write(fifo2_fd, output_buffer, strlen(output_buffer));
            if (bytes_written == -1) {
                exit(1);
            }

            close(fifo1_fd);
            close(fifo2_fd);
            exit(0);
        } else {
            if ((pid3 = fork()) == -1) {
                exit(1);
            }

            if (pid3 == 0) {
                int fifo2_fd = open(fifo2, O_RDONLY);

                if (fifo2_fd == -1) {
                    exit(1);
                }

                ssize_t bytes_read = read(fifo2_fd, output_buffer, BUFFER_SIZE);

                if (bytes_read == -1) {
                    exit(1);
                }

                ssize_t bytes_written = write(output_file, output_buffer, strlen(output_buffer));

                if (bytes_written == -1) {
                    exit(1);
                }

                close(fifo2_fd);
                exit(0);
            } else {
                wait(NULL);
                wait(NULL);
                wait(NULL);
            }
        }
    }

    close(input_file);
    close(output_file);
    unlink(fifo1);
    unlink(fifo2);
    return 0;
}
