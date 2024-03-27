#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <libc.h>

void handler(int signum, siginfo_t *info, void *context)
{
    printf("Received signal: %d from PID: %d with action: %d\n", signum, info->si_pid, info->si_value.sival_int);
    kill(info->si_pid, SIGUSR1);
}

int main(int argc, char *argv[])
{
    printf("Catcher PID: %d\n", getpid());
    printf("Listening...\n");

    struct sigaction act;
    act.__sigaction_u.__sa_sigaction = &handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);
}