#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

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


int main() {
    const char *filename = "calc";
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Problem with opening file");
        return 1;
    }

    double start, end, h;
    read(fd, &start, sizeof(double));
    read(fd, &end, sizeof(double));
    read(fd, &h, sizeof(double));

    double result = integrate(h, start, end);

    close(fd);

    fd = open(filename, O_WRONLY);
    if (fd == -1) {
        perror("Problem with opening file");
        return 1;
    }
    write(fd, &result, sizeof(double));
    close(fd);

    return 0;
}