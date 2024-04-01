#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Sposób użycia: %s <PID_catchera>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t catcher_pid = atoi(argv[1]);
    int mode = 1; // Początkowy tryb pracy
    
    while (1) {
        // Przesłanie trybu pracy do catcher'a
        union sigval value;
        value.sival_int = mode;
        sigqueue(catcher_pid, SIGUSR1, value);
        
        // Oczekiwanie na potwierdzenie odbioru sygnału SIGUSR1 przez catcher'a
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigsuspend(&mask);
        
        // Wysyłanie kolejnego sygnału dopiero po uzyskaniu potwierdzenia
        mode = (mode % 3) + 1;
        
        // Warunek zakończenia programu
        if (mode == 3) {
            printf("Sender: Zakończenie pracy.\n");
            break;
        }
    }

    return EXIT_SUCCESS;
}
