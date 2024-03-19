#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int global = 0;
extern int errno;

int main(int argc, char *argv[])
{
    int local = 0;
    if (argc < 2)
    {
        printf("Too few arguments given.\n");
        return 1;
    }
    pid_t child_pid = fork();
    printf("Program name: %s\n", argv[0]);
    if (child_pid == 0)
    {
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        if (execl("bin/ls", "ls", argv[1], NULL) == -1)
        {
            printf("%d\n", errno);
        }
        exit(0);
    }
    else if (child_pid < 0)
    {
        perror("Fork function error");
    }
    else
    {
        int success;
        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", getpid(), child_pid);
        pid_t _ = waitpid(child_pid, &success, 0);
        printf("Child exit status %d\n", success);
        printf("Parent's local = %d, parent's global = %d\n", local, global);
    }
    return 0;
}