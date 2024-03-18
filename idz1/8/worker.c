#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "../common/mylib.h"

const int BUFF_SIZE = 5000;

int main(const int argc, char *argv[]) {
    char input_buffer[BUFF_SIZE];
    char output_buffer[BUFF_SIZE];
    char fifo1[] = "vovan";
    char fifo2[] = "antoha";

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

    return 0;
}
