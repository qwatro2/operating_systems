#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>


// global data
bool received_flag;                                                             // flag for 10

// 3rd party functions
void bin(int value, bool *binary_int) {                                         // convert decimal integer to binary
    int size = sizeof(value) * 8;
    for (int i = 0; i < size; ++i) {
        binary_int[size - 1 - i] = value >> i & 1;
    }
}

int input(const char *promt) {                                                  // input integer
    puts(promt);
    int result;
    scanf("%d", &result);
    return result;
}

// work-with-bits functions
void send_bit(int receiver_pid, bool bit) {
    received_flag = false;                                                      // bit wasn't received

    if (kill(receiver_pid, bit ? SIGUSR2 : SIGUSR1) == -1) {
        printf("sender::ERROR: %s\n", strerror(errno));                         // error
        exit(1);
    }

    while (!received_flag) {}                                                   // wait while bit not received
}

void bit_received() {                                                           // change state of flag
    received_flag = true;
}

// main
int main() {
    if (signal(SIGUSR2, bit_received) == SIG_ERR) {
        printf("sender:ERROR: %s\n", strerror(errno));                          // error
        return 1;
    }

    printf("sender::PID: %d\n", getpid());                                      // print this PID

    int receiver_pid = input("Enter receiver::PID:");                           // read another PID
    int value = input("Enter number to send:");                                 // read value to send
    int size = sizeof(value) * 8;

    bool binary_int[size];
    bin(value, binary_int);                                                     // convert decimal integer to binary

    for (int i = 0; i < size; ++i) {                                            // send bits
        send_bit(receiver_pid, binary_int[i]);
    }

    if (kill(receiver_pid, SIGINT) == -1) {
        printf("sender::ERROR: %s\n", strerror(errno));                         // error
        exit(1);
    }

    return 0;
}