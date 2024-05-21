#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Too few arguments given, expected 2 but got %d\n", argc);
        return 1;
    }

    if (atoi(argv[1]) == 0) {
        fprintf(stderr, "Couldnt convert %s to int\n", argv[1]);
        return 1;
    }

    const uint16_t port = atoi(argv[1]);

    int server;

    if ((server = socket(AF_INET, SOCK_STREAM, 0), server == -1)) {
        perror("Error while creating server\n");
        return 1;
    }

    struct sockaddr_in server_info;
    struct in_addr server_ip;

    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(port);
    server_ip.s_addr = INADDR_ANY;

    server_info.sin_addr = server_ip;

    if (bind(server, (struct sockaddr *)&server_info, sizeof(server_info)) == -1) {
        perror("Error while binding server\n");
        return 1;
    }

    if (listen(server, 5) == -1) {
        perror("Error while listening\n");
        return 1;
    }

    printf("Server is listening at port: %d...\n", port);

    close(server);

    return 0;
}