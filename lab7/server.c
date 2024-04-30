#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE_SIZE 256

struct message {
    long type;
    char text[MAX_MESSAGE_SIZE];
    int id;
};

typedef struct client {
    int queue_id;
    int client_id;
} Client;

void print_clients(Client c[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Client - ID: %d, QUEUE: %d\n", c[i].client_id, c[i].queue_id);
    }
}

int main() {
    const char *path = "/tmp";
    key_t server_key = ftok(path, 'S');
    int server_msgid = msgget(server_key, IPC_CREAT | 0666);
    if (server_msgid == -1) {
        perror("Error with creating server message queue");
        return 1;
    }
    printf("Server key: %d\n", server_key);
    printf("Server ID: %d\n", server_msgid);

    struct client clients[MAX_CLIENTS];
    int n = 0;
    while (1) {
        struct message msg;
        if (msgrcv(server_msgid, &msg, sizeof(msg.text), 1, 0) == -1) {
            perror("Error receiving message from client\n");
            return 1;
        }
        printf("ID: %d\n", msg.id);
        char *tok = strtok(msg.text, " ");
        if (strcmp(tok, "INIT") == 0) {
            if (n >= MAX_CLIENTS) {
                printf("Too many clients.\n");
                continue;
            }
            tok = strtok(NULL, " ");
            int client_key = atoi(tok);
            int client_queue = msgget(client_key, 0);
            if (client_queue == -1) {
                perror("Error creating client queue\n");
                return 1;
            }

            struct message res;
            res.type = 1;
            sprintf(res.text, "%d", n + 1);
            if (msgsnd(client_queue, &res, strlen(res.text) + 1, 0) == -1) {
                perror("Error sending user's ID\n");
                return 1;
            }

            clients[n].client_id = n + 1;
            clients[n].queue_id = client_queue;
            n++;
            print_clients(clients, n);
        }
        else {
            for (int i = 0; i < n; i++) {
                if (clients[i].client_id != msg.id) {
                    if (msgsnd(clients[i].queue_id, &msg, strlen(msg.text) + 1, 0) == -1) {
                        perror("Error sending message to user\n");
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
