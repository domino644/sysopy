#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments given.\n");
        return 1;
    }
    if (!atoi(argv[1]))
    {
        printf("Couldn't convert %s to int", argv[1]);
    }
    int process_no = atoi(argv[1]);
    for (int i = 0; i < process_no; i++)
    {
        if (fork() == 0)
        {
            printf("child id: %d, parent id: %d\n", getpid(), getppid());
            exit(0);
        }
    }
    while (wait(NULL) != -1)
        ;
    printf("Number of processes: %d\n", process_no);
    return 0;
}