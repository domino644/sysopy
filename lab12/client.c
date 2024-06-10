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


void *sender();
void *receiver();

int sockfd;

char *username;
size_t username_len = 100;

int done = 0;

char send_buff[1024];
char receive_buff[1024];

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


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0), sockfd == -1)) {
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

    if (connect(sockfd, (struct sockaddr *)&server_info, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Desired username: ");
    getline(&username, &username_len, stdin);
    strncpy(send_buff, username, username_len);
    if (send(sockfd, send_buff, strlen(send_buff), 0) == -1) {
        perror("send");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[2];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&threads[0], &attr, sender, NULL);
    pthread_create(&threads[1], &attr, receiver, NULL);

    while (!done);

    close(sockfd);
    exit(EXIT_SUCCESS);
}


void *sender() {
    while (1) {
        bzero(send_buff, BUFSIZ);
        fgets(send_buff, BUFSIZ, stdin);


        if (send(sockfd, send_buff, strlen(send_buff), 0) == -1) {
            perror("send");
            done = 1;
            pthread_exit(NULL);
        }

        if (strcmp(send_buff, "STOP\n") == 0) {
            done = 1;
            pthread_exit(NULL);
        }
    }
}

void *receiver() {
    int nbytes;

    while (1) {
        bzero(receive_buff, BUFSIZ);

        if ((nbytes = recv(sockfd, receive_buff, BUFSIZ - 1, 0)) == -1) {
            perror("recv");
            done = 1;
            pthread_exit(NULL);
        }

        receive_buff[nbytes] = '\0';
        printf("%s\n", receive_buff);
    }
}