#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define DEFAULT_PORT 8080
#define DEFAULT_IP "127.0.0.1"

void *listen_handler(void *arg);

int main(int argc, char *argv[]) {
    uint16_t port;
    char *ip;
    if (argc == 3) {
        if (atoi(argv[2]) == 0) {
            fprintf(stderr, "Couldnt convert %s to int\n", argv[1]);
            return 1;
        }
        port = atoi(argv[2]);
        ip = argv[1];
    }
    else if (argc == 2) {
        port = DEFAULT_PORT;
        ip = argv[1];
    }
    else {
        ip = DEFAULT_IP;
        port = DEFAULT_PORT;
    }

    int server_fd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0), server_fd == -1)) {
        perror("Error while creating server\n");
        return 1;
    }

    struct sockaddr_in server_info;

    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(port);
    int s = inet_pton(AF_INET, ip, &server_info.sin_addr.s_addr);
    if (s <= 0) {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(server_fd, &server_info, sizeof(server_info)) != 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    pthread_t listen_thread;
    pthread_create(&listen_thread, NULL, listen_handler, &server_fd);

    printf("Type a message...\n");
    char *line = NULL;
    size_t lineSize = 0;
    char buff[1024];
    while (1) {
        ssize_t count = getline(&line, &lineSize, stdin);
        line[count - 1] = '\0';
        int saved = snprintf(buff, 1024, "%s", line);
        if (saved > 0) {
            send(server_fd, buff, strlen(buff), 0);
            if (strcmp(buff, "STOP") == 0) {
                break;
            }
        }

    }

    close(server_fd);
    return 0;
}


void *listen_handler(void *arg) {
    int *fd = (int *)arg;
    char buff[1024];


    while (1) {
        ssize_t received = recv(*fd, buff, 1024, 0);
        if (received > 0) {
            buff[received] = 0;
            printf("%s\n", buff);
        }
        else {
            break;
        }
    }
    close(*fd);
    pthread_exit(NULL);
}