#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

volatile int mode = 1;
volatile int changes = 0;

void handler(int signum, siginfo_t *info, void *context)
{
    printf("Received signal: %d from PID: %d with action: %d\n", signum, info->si_pid, info->si_value.sival_int);
    if (mode != info->si_value.sival_int)
    {
        changes++;
    }
    mode = info->si_value.sival_int;
    switch (mode)
    {
    case 1:
        for (int i = 1; i <= 100; i++)
        {
            printf("%d\n", i);
        }
        break;
    case 2:
        printf("Changes: %d\n", changes);
        break;
    case 3:
        exit(0);
        break;
    default:
        break;
    }
    if (kill(info->si_pid, SIGUSR1) == -1)
    {
        printf("Error with kill(): %d\n", errno);
        exit(1);
    }
    printf("Signal sent to PID: %d\n", info->si_pid);
}

int main(int argc, char *argv[])
{
    printf("Catcher PID: %d\n", getpid());
    printf("Listening...\n");

    struct sigaction act;
    act.sa_sigaction = &handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    while (1)
        ;
}