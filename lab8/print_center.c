#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>

#define MAX_USERS 10
#define MAX_PRINTERS 5
#define MAX_QUEUE_SIZE 20
#define TEXT_SIZE 10

struct shared_memory {
    char print_queue[MAX_QUEUE_SIZE][TEXT_SIZE];
    int queue_front;
    int queue_rear;
};

void wait_semaphore(int sem_id, int sem_num) {
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = -1;
    sem_buf.sem_flg = SEM_UNDO;
    semop(sem_id, &sem_buf, 1);
}

void signal_semaphore(int sem_id, int sem_num) {
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = 1;
    sem_buf.sem_flg = SEM_UNDO;
    semop(sem_id, &sem_buf, 1);
}

int main() {
    key_t key = ftok("printer_system", 65);
    int shmid = shmget(key, sizeof(struct shared_memory), IPC_CREAT | 0666);
    struct shared_memory *shared_mem = (struct shared_memory *)shmat(shmid, NULL, 0);
    union semun sem_union;
    int sem_id = semget(key, 3, IPC_CREAT | 0666);

    sem_union.val = 1;
    semctl(sem_id, 0, SETVAL, sem_union);

    sem_union.val = MAX_QUEUE_SIZE;
    semctl(sem_id, 1, SETVAL, sem_union);

    sem_union.val = 1;
    semctl(sem_id, 2, SETVAL, sem_union);

    srand(time(NULL));

    for (int i = 0; i < MAX_USERS; i++) {
        if (fork() == 0) {
            while (1) {
                char text[TEXT_SIZE];
                for (int j = 0; j < TEXT_SIZE; j++) {
                    text[j] = 'a' + rand() % 26;
                }

                wait_semaphore(sem_id, 1);
                wait_semaphore(sem_id, 0);

                if ((shared_mem->queue_rear + 1) % MAX_QUEUE_SIZE != shared_mem->queue_front) {
                    strcpy(shared_mem->print_queue[shared_mem->queue_rear], text);
                    shared_mem->queue_rear = (shared_mem->queue_rear + 1) % MAX_QUEUE_SIZE;
                }

                signal_semaphore(sem_id, 0);
                signal_semaphore(sem_id, 2);

                sleep(rand() % 5 + 1);
            }
        }
    }

    for (int i = 0; i < MAX_PRINTERS; i++) {
        if (fork() == 0) {
            while (1) {
                wait_semaphore(sem_id, 2);
                wait_semaphore(sem_id, 0);

                if (shared_mem->queue_front != shared_mem->queue_rear) {
                    int i = 0;
                    while (i < TEXT_SIZE) {
                        if (shared_mem->print_queue[shared_mem->queue_front][i]) {
                            printf("%c\n", shared_mem->print_queue[shared_mem->queue_front][i]);
                        }
                        i++;
                        sleep(1);
                    }
                    shared_mem->queue_front = (shared_mem->queue_front + 1) % MAX_QUEUE_SIZE;
                }

                signal_semaphore(sem_id, 0);
                signal_semaphore(sem_id, 1);

                sleep(1);
            }
        }
    }


    for (int i = 0; i < MAX_USERS + MAX_PRINTERS; i++) {
        wait(NULL);
    }


    shmdt(shared_mem);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, sem_union);

    return 0;
}
