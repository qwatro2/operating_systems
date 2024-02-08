#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>

#define num uint64_t

// function for counting n!
num factorial(num n) {
    num result = 1;
    for (num i = 2; i <= n; ++i) {
        if (result * i < result) {
            fprintf(stderr, "Overflow of n! in n = %lu\n", n);
            return 0;
        }

        result *= i;
    }

    return result;
}

// function for counting F_n
num fibbonacci(num n) {
    if (n <= 1) {
        return n;
    }

    num previous = 0, current = 1, tmp = -1;
    for (unsigned int i = 2; i <= n; ++i) {
        if (previous + current < current) {
            fprintf(stderr, "Overflow of F_n in n = %lu\n", n);
            return 0;
        }
        tmp = previous;
        previous = current;
        current = current + tmp;
    }

    return current;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {                                                                    // there is no arguments except executable file name
        fprintf(stderr, "Enter N as a command-line argument\n");
        return -1;
    }

    if (argv[1][0] == '-') {
        fprintf(stderr, "Enter positive N as a command-line argument\n");
        return -1;
    }

    num n = atoi(argv[1]);

    pid_t process = fork();                                                             // fork process

    if (process < 0) {                                                                  // error during process creating
        fprintf(stderr, "Error during process creating\n");
        return -1;
    }

    if (process == 0) {                                                                 // process for n! counting
        printf("Child process, PID: %d;\tParent process: %d\n", getpid(), getppid());
        num fact = factorial(n);
        if (fact == 0) {  // overflow
            return -1;
        }
        printf("%lu! = %lu\n", n, fact);
    } else {                                                                            // process for F_n counting
        printf("Parent process, PID: %d;\tChild process: %d\n", getpid(), process);
        num fibbo = fibbonacci(n);
        if (n == 0 || fibbo > 0) {  // overflow
            printf("F_%lu = %lu\n", n, fibbo);
        }        
        waitpid(process, NULL, 0);                                                      // wait while child process end
        execlp("ls", "ls", "-l", (char *) NULL);                                        // print inforamtion about current dir
    }
    
    return 0;
}
