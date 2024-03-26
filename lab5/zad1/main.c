#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

void handler(int signum)
{
    printf("Handling signal: %d\n", signum);
};

void mask_handler(int signum)
{
    sigset_t sig_pend;
    if (sigpending(&sig_pend) != 0)
    {
        perror("sigpending error");
    }
    if (sigismember(&sig_pend, signum))
    {
        printf("%d is pending\n", signum);
    }
    else
    {
        printf("%d is not pending\n", signum);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("Please specify signal action: none, ignore, handler, mask\n");
        return 1;
    }
    if (strcmp(argv[1], "none") != 0 && strcmp(argv[1], "ignore") != 0 && strcmp(argv[1], "handler") != 0 && strcmp(argv[1], "mask") != 0)
    {
        perror("Unknown argument, please choose one of: none, ignore, handler, mask\n");
        return 1;
    }

    char *argument = argv[1];
    if (strcmp(argument, "ignore") == 0)
    {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(argument, "handler") == 0)
    {
        signal(SIGUSR1, handler);
    }
    else if (strcmp(argument, "mask") == 0)
    {
        sigset_t new_blocked_signals;
        sigset_t old_blocked_signals;
        sigemptyset(&new_blocked_signals);
        sigaddset(&new_blocked_signals, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &new_blocked_signals, &old_blocked_signals) < 0)
            perror("Nie udało się zablokować sygnału");
        struct sigaction act;
        act.sa_handler = mask_handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);
    }

    if (raise(SIGUSR1) == -1)
    {
        perror("Error while sending signal");
        return 1;
    }
    return 0;
}