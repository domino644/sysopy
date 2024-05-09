#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PIPE "./squareFIFO"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Not a suitable number of program parameters\n");
    return(1);
  }
  mkfifo(PIPE, 0666);
  int fd = open(PIPE, O_WRONLY);
  int num = atoi(argv[1]);
  printf("num: %d\n", num);
  write(fd, &num, sizeof(int));

  close(fd);

  unlink(PIPE);


  //utworz potok nazwany pod sciezka reprezentowana przez PIPE
  //zakladajac ze parametrem wywolania programu jest liczba calkowita
  //zapisz te wartosc jako int do potoku i posprzataj


  return 0;
}
