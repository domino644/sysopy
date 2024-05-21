#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>


#define REINDEERS 9
#define DRIVE_BYS 4

int ids[9];

pthread_mutex_t reindeers_mutex[9] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
};

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;

int reindeers_on_duty = 0;
pthread_mutex_t duty_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t christmas = PTHREAD_COND_INITIALIZER;

pthread_t santa_thread;
pthread_t reindeers_threads[9];

void release_reindeers() {
    for (int i = 0; i < REINDEERS; i++) {
        pthread_mutex_unlock(&reindeers_mutex[i]);
    }
}

void retire_reindeers() {
    for (int i = 0; i < REINDEERS; i++) {
        pthread_cancel(reindeers_threads[i]);
    }
}

void *santa_fun(void *arg) {
    for (int i = 0; i < DRIVE_BYS; i++) {
        pthread_cond_wait(&christmas, &santa_mutex);
        printf("Mikołaj: Budze sie\n");
        unsigned int s = rand() % 3 + 2;
        printf("Mikołaj: Dostarczam zabawki\n");
        sleep(s);
        release_reindeers();
        printf("Zasypiam\n");
    }
    retire_reindeers();
    return NULL;
}

void *reindeer_fun(void *arg) {
    int id = *(int *)arg;

    pthread_mutex_lock(&reindeers_mutex[id]);

    while (1) {
        unsigned int s = rand() % 6 + 5;
        sleep(s);
        pthread_mutex_lock(&duty_mutex);
        reindeers_on_duty++;
        printf("Renifer: czeka %d reniferow, ID: %d\n", reindeers_on_duty, id);

        if (reindeers_on_duty == REINDEERS) {
            printf("Renifer: Budzę Mikołaja, ID: %d\n", id);
            pthread_cond_signal(&christmas);
            reindeers_on_duty = 0;
        }

        pthread_mutex_unlock(&duty_mutex);

        pthread_mutex_lock(&reindeers_mutex[id]);
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    pthread_create(&santa_thread, NULL, santa_fun, NULL);

    for (int i = 0; i < REINDEERS; i++) {
        ids[i] = i;
        pthread_create(&reindeers_threads[i], NULL, reindeer_fun, &ids[i]);
    }

    pthread_join(santa_thread, NULL);
    for (int i = 0; i < REINDEERS; i++) {
        pthread_join(reindeers_threads[i], NULL);
    }

    return 0;
}