#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

double fun(double x)
{
    return 4 / (x * x + 1);
}

double integrate(double h, double start, double end)
{
    double output = 0;
    double i = start;
    while (i + h <= end)
    {
        output += fun(i + h) + fun(i);
        i += h;
    }
    if (i < end)
    {
        output += fun(end) + fun(i);
    }
    return output * h / 2;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("2 arguments expected, %d given\n", argc - 1);
        return 1;
    }
    double h;
    if (sscanf(argv[1], "%lf", &h) == EOF)
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
    double segment_size = 1.0 / n;
    double output = 0;

    printf("segment size: %lf\n", segment_size);
    for (double i = 0; i + segment_size <= 1; i += segment_size)
    {
        pid_t PID;
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("Error with pipe");
            return 1;
        }

        PID = fork();

        if (PID < 0)
        {
            perror("Error with fork");
            return 1;
        }

        if (PID == 0)
        {
            close(pipefd[0]);
            double partial_integral = integrate(h, i, i + segment_size);
            write(pipefd[1], &partial_integral, sizeof(double));
            close(pipefd[1]);
            return 0;
        }
        else
        {
            close(pipefd[1]);
            double partial_integral;
            read(pipefd[0], &partial_integral, sizeof(double));
            close(pipefd[0]);
            output += partial_integral;
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("Integral: %f\n", output);

    return 0;
}