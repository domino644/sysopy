#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>

#define L_PYTAN 10

int main(void) {
    int status, gniazdo, i;
    struct sockaddr_in ser, cli;
    char buf[200];
    char pytanie[] = "abccbahhff";

    gniazdo = socket(AF_INET, SOCK_STREAM, 0);
    if (gniazdo == -1) { printf("blad socket\n"); return 0; }

    memset(&ser, 0, sizeof(ser));
    //nawiaz polaczenie z IP 127.0.0.1 i usluga na porcie takim samym jak ustaliles w serwerze, zwroc status
    //...

    ser.sin_family = AF_INET;
    ser.sin_port = htons(8080);
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");

    status = connect(gniazdo, (const struct sockaddr *)&ser, sizeof(ser));

    if (status < 0) { printf("blad 01\n"); return 0; }
    for (i = 0; i < L_PYTAN; i++) {
        status = write(gniazdo, pytanie + i, 1);
        //odczytaj dane otrzymane z sieci, wpisz do tablicy 'buf' i wyświetl na standardowym wyjściu (ekranie)
        //...

        recv(gniazdo, buf, 200, 0);
        printf("%s\n", buf);

    }
    printf("\n");

    close(gniazdo);
    printf("KONIEC DZIALANIA KLIENTA\n");
    return 0;
}

