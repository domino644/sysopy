#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 256

struct message {
    long type;
    char text[MAX_MESSAGE_SIZE];
    int id;
};

int client_id;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Too few arguments, %d expected but %d given. \n", 1, argc - 1);
        return 1;
    }
    key_t server_key;
    if (atoi(argv[1]) == 0) {
        printf("Cannot convert, %s into integer.\n", argv[1]);
        return 1;
    }
    server_key = atoi(argv[1]);
    const char *path = "/tmp";

    key_t client_key = ftok(path, getpid());
    int client_queue = msgget(client_key, IPC_CREAT | 0666);
    if (client_queue == -1) {
        perror("Error creating client queue\n");
        return 1;
    }

    int server_queue = msgget(server_key, 0);
    if (server_queue == -1) {
        perror("Error accessing server queue\n");
        return 1;
    }
    printf("Server -- ID: %d, key: %d\n", server_queue, server_key);
    printf("Client -- ID: %d, key: %d\n", client_queue, client_key);

    struct message init_msg;
    init_msg.type = 1;
    sprintf(init_msg.text, "INIT %d", client_key);
    if (msgsnd(server_queue, &init_msg, strlen(init_msg.text) + 1, 0) == -1) {
        perror("Error sending INIT message\n");
        return 1;
    }


    struct message res;
    if (msgrcv(client_queue, &res, sizeof(res.text), 0, 0) == -1) {
        perror("Error receiving ID message\n");
        return 1;
    }
    if (strcmp(res.text, "0") != 0 && atoi(res.text) == 0) {
        perror("Error converting ID to integer\n");
        return 1;
    }
    client_id = atoi(res.text);

    printf("I got initialized with ID: %d!\n", client_id);

    pid_t PID = fork();
    if (PID == -1) {
        perror("Error with fork\n");
        return 1;
    }
    if (PID == 0) {
        while (1) {
            //receiver - child
            struct message res;
            if (msgrcv(client_queue, &res, sizeof(res.text), 0, 0) == -1) {
                perror("Error receiving a message\n");
                return 1;
            }
            printf("User %d: %s\n", res.id, res.text);
        }
    }
    else {
        //sender - parent
        while (1) {
            struct message msg;
            fgets(msg.text, sizeof(msg.text), stdin);
            msg.type = 1;
            msg.id = client_id;
            if (msgsnd(server_queue, &msg, strlen(msg.text) + 1, 0) == -1) {
                perror("Error sending message\n");
                return 1;
            }
        }
    }
    return 0;
}