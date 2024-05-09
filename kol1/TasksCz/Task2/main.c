#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv) {
    if (argc == 2) {
        char *filename1 = argv[1];
        int fd[2];
        pipe(fd);
        pid_t pid = fork();
        if (pid == 0) {
            //  zamknij deskryptor do zapisu i wykonaj program sort na filename1
            //  w przypadku błędu zwróć 3
            if (close(fd[1]) == -1) {
                return 3;
            }
            execl("/usr/bin/sort", filename1, NULL);

            //  koniec
        }
        else {
            close(fd[0]);
        }
    }
    else if (argc == 3) {
        char *filename1 = argv[1];
        char *filename2 = argv[2];
        int fd[2];
        //  otwórz plik filename2 z prawami dostępu rwxr--r--, 
        //  jeśli plik istnieje otwórz go i usuń jego zawartość
        if (open(filename2, O_RDWR, 0644) < 0) {
            perror("not exists");
        }
        else {
            fopen(filename2, "w");
        }


        //  koniec
        pipe(fd);
        pid_t pid = fork();
        if (pid == 0) {
            //  zamknij deskryptor do zapisu,
            //  przekieruj deskryptor standardowego wyjścia na deskryptor pliku filename2 i zamknij plik,
            //  wykonaj program sort na filename1
            //  w przypadku błędu zwróć 3.
            close(fd[1]);

            int filefd = open(filename2, O_WRONLY, 0666);
            if (filefd < 0) {
                perror("open");
            }
            close(1);
            dup(filefd);

            execl("/usr/bin/sort", filename1, NULL);


            //  koniec
        }
        else {
            close(fd[0]);
        }
    }
    else
        printf("Wrong number of args! \n");

    return 0;
}