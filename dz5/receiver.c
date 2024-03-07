#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

// global data
int value = 0;                                                          // result
int sender_pid;                                                         // another pid
int received_bits = 0;                                                  // number of bits

// 3rd party function
int input(const char *promt) {                                          // input integer
    puts(promt);
    int result;
    scanf("%d", &result);
    return result;
}

// work-with-bits functions
void receive_bit(int signum) {                                          // receive one bit
    if (signum != SIGUSR1 && signum != SIGUSR2) {
        return;
    }

    bool bit = signum == SIGUSR2;
    ++received_bits;
    value = (value << 1) | bit;                                         // tranform result

    if (kill(sender_pid, SIGUSR2) == -1) {                              // error
        printf("receiver::ERROR: %s\n", strerror(errno));
        exit(1);
    }
}

void hdl(int signum) {
    printf("Received number: %d\n", value);                             // print result
    exit(0);
}

// main
int main() {
    if (signal(SIGUSR1, receive_bit) == SIG_ERR || signal(SIGUSR2, receive_bit) == SIG_ERR ||
        signal(SIGINT, hdl) == SIG_ERR) {
        printf("receiver::ERROR: %s\n", strerror(errno));               // error
        return 1;
    }

    printf("receiver::PID: %d\n", getpid());                            // print this PID
    sender_pid = input("Enter sender::PID:");                           // read another PID
    while (true) {}                                                     // wait while all bits passed
}