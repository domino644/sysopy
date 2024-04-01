#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Zmienna przechowująca tryb pracy
volatile int mode = 1;

// Funkcja obsługująca sygnał SIGUSR1
void sigusr1_handler(int signum, siginfo_t *info, void *context) {
    printf("Catcher: Odebrano sygnał SIGUSR1 od sendera.\n");
    
    // Pobranie trybu pracy przesłanego przez sendera
    mode = info->si_value.sival_int;

    printf("Sender's PID: %d\n", info->si_pid);
    
    // Wysłanie potwierdzenia odbioru sygnału SIGUSR1 do sendera
    kill(info->si_pid, SIGUSR1);
}

// Funkcja obsługująca sygnał SIGINT (Ctrl+C)
void sigint_handler(int signum) {
    printf("Catcher: Odebrano sygnał SIGINT. Koniec pracy.\n");
    exit(EXIT_SUCCESS);
}

int main() {
    // Ustawienie obsługi sygnałów
    struct sigaction sa_usr1, sa_int;
    sa_usr1.sa_flags = SA_SIGINFO;
    sa_usr1.sa_sigaction = sigusr1_handler;
    sigaction(SIGUSR1, &sa_usr1, NULL);
    
    sa_int.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa_int, NULL);

    // Wypisanie PID catcher'a
    printf("Catcher PID: %d\n", getpid());

    // Czekanie na sygnały SIGUSR1
    while (1) {
        pause();
        printf("Catcher: Tryb pracy: %d\n", mode);
        if (mode == 3) {
            printf("Catcher: Zakończenie pracy.\n");
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
