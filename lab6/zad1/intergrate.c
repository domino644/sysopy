#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

struct data
{
    double start;
    double end;
    double h;
    double (*fun)(double x);
    double output;
};

double fun(double x)
{
    return 4 / (x * x + 1);
}

double integrate(double h, double start, double stop, double (*fun)(double))
{
    double output = 0;
    double i = start;
    while (i + h <= end)
    {
        output += fun(i + h) - fun(i);
        i += h;
    }
    if (i < end)
    {
        output += fun(end) - fun(i);
    }
    return output * h;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("2 arguments expected, %d given\n", argc - 1);
        return 1;
    }
    double h;
    if (sscanf(argv[1], "%lf", &d) == EOF)
    {
        printf("Error while converting %s to double\n", argv[1]);
        return 1;
    }
    int n;
    n = atoi(argv[2]);
    if (n <= 0)
    {
        printf("Error while converting %s to integer\n", argv[2]);
        return 1;
    }
    double segment_size = 1 / n;
    for (double i = 0; i + segment_size <= 1; i += segment_size)
    {
        pid_t PID = fork();
        if (PID < 0)
        {
            perror("Error with fork");
        }
        else if (PID > 0)
        {
            int fd[2];
            if (pipe(fd) < 0)
            {
                exit(1);
            }
            struct data req;
            req.start = i;
            req.end = i + segment_size;
            req.h = h;
            req.fun = &fun;
            req.output = NULL;
            close(fd[0]);
            write(fd[1], req, sizeof(struct data));
        }
        else
        {
            close(fd[1]);
            struct data res;
            while (read(fd[0], &res, sizeof(struct data)) > 0)
            {
            }
        }
    }

    return 0;
}