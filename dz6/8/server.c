#include "../message.h"
#include <stdio.h>
#include <sys/shm.h>

int main() {
    const int shmid = shmget(SHM_KEY, sizeof(message_t), SHM_PERMS | IPC_CREAT);
    
    if (shmid < 0) {
        printf("[ОШИБКА] при создании сегмента общей памяти с ключом %d\n", SHM_KEY);
        return 1;
    }

    message_t *message = shmat(shmid, NULL, 0);

    if (message == (void *) -1) {
        printf("[ОШИБКА] при присоединении к сегменту общей памяти с id %d\n", shmid);
        return 1;
    }

    message->type = MESSAGE_TYPE_DEFAULT;
    
    while (1) {
        if (message->type == MESSAGE_TYPE_DISCONNECT) {
            printf("[ОШИБКА] потеряно соединение с клиентом\n");
            break;
        }

        if (message->type == MESSAGE_TYPE_SEND) {
            printf("[УСПЕХ] получено число: %s\n", message->data);
            message->type = MESSAGE_TYPE_DEFAULT;
        }
    }

    if (shmdt(message) < 0) {
        printf("[ОШИБКА] при отсоединении от сегмента общей памяти с id %d\n", shmid);
        return 1;
    }

    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("[ОШИБКА] при удалении сегмента общей памяти с ключом %d\n", SHM_KEY);
        return 1;
    }

    return 0;
}