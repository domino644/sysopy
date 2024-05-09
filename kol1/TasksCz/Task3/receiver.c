#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define PIPE "./squareFIFO"

int main() {
    int val = 0;
    /***********************************
    * odczytaj z potoku nazwanego PIPE zapisana tam wartosc i przypisz ja do zmiennej val
    * posprzataj
    ************************************/

    int fd = open(PIPE, O_RDONLY);
    read(fd, &val, sizeof(int));

    printf("val: %d\n", val);

    close(fd);

    printf("%d square is: %d\n", val, val * val);
    return 0;
}
