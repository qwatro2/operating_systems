#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "../common/mylib.h"

const int BUFF_SIZE = 5000;

int main(const int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input> <output>", argv[0]);
        exit(1);
    }

    char input_buffer[BUFF_SIZE];
    char output_buffer[BUFF_SIZE];

    char fifo1[] = "vovan";
    char fifo2[] = "antoha";

    if ((mkfifo(fifo1, 0666) == -1) || (mkfifo(fifo2, 0666) == -1)) {
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        exit(1);
    }

    if (pid == 0) {
        int fd1 = open(fifo1, O_WRONLY);

        if (fd1 == -1) {
            exit(1);
        }

        int fd2 = open(fifo2, O_RDONLY);

        if (fd2 == -1) {
            exit(1);
        }
        
        int input_file = open(argv[1], O_RDONLY);

        if (input_file == -1) {
            exit(1);
        }
        
        ssize_t bytes_read = read(input_file, input_buffer, BUFF_SIZE);
        
        if (bytes_read == -1) {
            exit(1);
        }
        
        ssize_t bytes_written = write(fd1, input_buffer, bytes_read);
        
        if (bytes_written == -1) {
            exit(1);
        }
        
        bytes_read = read(fd2, output_buffer, BUFF_SIZE);
        
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
        close(fd1);
        close(fd2);
        exit(0);
    } else {
        int fd1 = open(fifo1, O_RDONLY);

        if (fd1 == -1) {
            exit(1);
        }

        int fd2 = open(fifo2, O_WRONLY);

        if (fd2 == -1) {
            exit(1);
        }
        
        ssize_t bytes_read = read(fd1, input_buffer, BUFF_SIZE);
        
        if (bytes_read == -1) {
            exit(1);
        }
        
        solution(input_buffer, output_buffer);
        
        ssize_t bytes_written = write(fd2, output_buffer, strlen(output_buffer));

        if (bytes_written == -1) {
            exit(1);
        }
        
        close(fd1);
        close(fd2);
        wait(NULL);
    }
    
    unlink(fifo1);
    unlink(fifo2);

    return 0;
}
