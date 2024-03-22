#include "../message.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

int shmid;
message_t *message;

void sigint_handler(int signum) {
    message->type = MESSAGE_TYPE_DISCONNECT;

    if (shmdt(message) < 0) {
        printf("[ОШИБКА] при отсоединении от сегмента общей памяти с id %d\n", shmid);
        exit(1);
    }

    exit(0);
}

int main() {
    shmid = shmget(SHM_KEY, sizeof(message_t), 0);

    if (shmid < 0) {
        printf("[ОШИБКА] при доступе к сегменту общей памяти с ключом %d\n", SHM_KEY);
        return 1;
    }

    message = shmat(shmid, NULL, 0);

    if (message == (void *) -1) {
        printf("[ОШИБКА] при присоединении к сегменту общей памяти с id %d\n", shmid);
        return 1;
    }

    signal(SIGINT, sigint_handler);
    srand(time(NULL));

    while (1) {
        const int random_number = rand() % 1000 + 1;

        sprintf(message->data, "%d", random_number);
        message->type = MESSAGE_TYPE_SEND;
        printf("[УСПЕХ] отправлено число: %d\n", random_number);

        sleep(1);
    }

    return 0;
}