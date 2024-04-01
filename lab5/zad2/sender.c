#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#define _XOPEN_SOURCE 500

void handler(int signum)
{
    printf("Received signal: %d\n", signum);
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

    if (sigqueue(PID, SIGUSR1, value) == -1)
    {
        printf("Error with siqque, %d\n", errno);
        return 1;
    }
    signal(SIGUSR1, handler);

    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGUSR1);
    sigsuspend(&signals);

    while (1)
    {
        scanf("Action: %d", &action);
        value.sival_int = action;
        if (sigqueue(PID, SIGUSR1, value) == -1)
        {
            printf("Error with sigque, %d\n", errno);
            return 1;
        }
        sigsuspend(&signals);
    }
    return 0;
}