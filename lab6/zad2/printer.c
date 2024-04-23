#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("1 argument expected, %d given\n", argc - 1);
        return 1;
    }
    double h;
    if (sscanf(argv[1], "%lf", &h) == EOF)
    {
        printf("Error while converting %s to double\n", argv[1]);
        return 1;
    }
    const char *filename = "calc";
    mkfifo(filename, 0666);
    printf("CZKEAM\n");

    int fd = open(filename, O_WRONLY);
    if (fd == -1) {
        perror("Problem with opening file");
        return 1;
    }

    double start = 0.0, end = 1.0;

    write(fd, &start, sizeof(double));
    write(fd, &end, sizeof(double));
    write(fd, &h, sizeof(double));

    close(fd);

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Problem with opening file");
        return 1;
    }


    double result;
    read(fd, &result, sizeof(double));

    close(fd);
    unlink(filename);

    printf("Integral: %lf\n", result);
    return 0;
}