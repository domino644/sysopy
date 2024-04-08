#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

void handler(int signum)
{
    printf("Received signal: %d\n", signum);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("At least 2 arguments expected, %d given\n", argc - 1);
        return 1;
    }
    pid_t PID = (pid_t)atoi(argv[1]);
    int action = atoi(argv[2]);
    union sigval value;
    value.sival_int = action;

    signal(SIGUSR1, &handler);

    if (sigqueue(PID, SIGUSR1, value) == -1)
    {
        printf("Error with siqque, %d\n", errno);
        return 1;
    }

    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigsuspend(&signals);
}