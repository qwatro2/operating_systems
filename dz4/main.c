#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

const int buffer_size = 256;
const int default_mode = 0666;


int main(int argc, const char *argv[]) {
    if (argc < 3) {                                                                                             // wrong arguments number
        fprintf(stderr, "use %s <input> <output>\t to write content from <input> to <output>\n", argv[0]);
        exit(1);
    }
    
    char buffer[buffer_size];                                                                                   // buffer of fixed little size

    int input_fd = open(argv[1], O_RDONLY, default_mode);                                                       // open input file

    if (input_fd == -1) {                                                                                       // error during opening input file
        fprintf(stderr, "Error during opening %s\n", argv[1]);
        exit(1);
    }

    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, default_mode);                                  // open outpit file

    ssize_t bytes_read = 0;
    do {
        bytes_read = read(input_fd, buffer, buffer_size);                                                       // read bytes from file to buffer
        if (bytes_read == -1) {                                                                                 // error during reading input file
            fprintf(stderr, "Error during reading %s\n", argv[1]);
            exit(1);
        }
        write(output_fd, buffer, bytes_read);                                                                   // write bytes to file to buffer
    } while (bytes_read == buffer_size);

    struct stat input_st;

    if (stat(argv[1], &input_st) != 0) {                                                                        // error during checking flags of input file
        fprintf(stderr, "Error during checking flags of %s\n", argv[1]);
        exit(1);
    }

    close(input_fd);                                                                                            // close input file

    if (fchmod(output_fd, input_st.st_mode) == -1) {                                                            // error during change flag of output file
        fprintf(stderr, "Error during changing mode of %s\n", argv[2]);
        exit(1);
    }

    close(output_fd);                                                                                           // close output file
} 