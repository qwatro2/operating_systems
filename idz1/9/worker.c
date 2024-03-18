#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "../common/mylib.h"

const int BUFF_SIZE = 128;

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

    ssize_t bytes_read;
    ssize_t bytes_written;
    char big_buffer_input[BUFF_SIZE * 100];
    char big_buffer_output[BUFF_SIZE * 100];

    while ((bytes_read = read(fd1, input_buffer, BUFF_SIZE)) > 0) {
        strncat(big_buffer_input, input_buffer, bytes_read);
    }

    solution(big_buffer_input, big_buffer_output);

    size_t total_bytes = strlen(big_buffer_output);
    size_t bytes_sent = 0;

    while (bytes_sent < total_bytes) {
        size_t bytes_to_write = total_bytes - bytes_sent;

        if (bytes_to_write > BUFF_SIZE) {
            bytes_to_write = BUFF_SIZE;
        }

        strncpy(output_buffer, big_buffer_output + bytes_sent, bytes_to_write);
        bytes_written = write(fd2, output_buffer, bytes_to_write);

        if (bytes_written == -1) {
            exit(1);
        }

        bytes_sent += bytes_written;
    }

    close(fd1);
    close(fd2);

    return 0;
}
