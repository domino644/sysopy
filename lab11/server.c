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

char *reject_msg = "Connection rejected, too many clients\n";

typedef struct client_info {
    int id;
    int fd;
    struct sockaddr address;
}client_info;

typedef struct node {
    struct client_info data;
    struct node *next;
}node;

struct node *head = NULL;

void deletenode(int key);
void insertatbegin(client_info data);

client_info *accept_client(int server_fd);
void listen_to_client(client_info *client);
void *thread_handler(void *arg);
void *pinger(void *arg);

int handle_message(char msg[], client_info *client);

pthread_mutex_t clients_mutex;
int clients_number = 0;


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
    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

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


    if (bind(server_fd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1) {
        perror("Error while binding server\n");
        return 1;
    }

    if (listen(server_fd, 10) == -1) {
        perror("Error while listening\n");
        return 1;
    }

    printf("Server is listening at %s:%d...\n", ip, port);

    pthread_t pinger_thread;
    pthread_create(&pinger_thread, NULL, pinger, NULL);

    while (1) {
        client_info *client = accept_client(server_fd);

        listen_to_client(client);
    }

    shutdown(server_fd, SHUT_RDWR);

    return 0;
}

client_info *accept_client(int server_fd) {
    struct sockaddr client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int client_fd = accept(server_fd, &client_addr, &client_addr_size);

    client_info *client = malloc(sizeof(client_info));
    client->address = client_addr;
    client->fd = client_fd;
    clients_number++;
    client->id = clients_number;
    pthread_mutex_lock(&clients_mutex);
    insertatbegin(*client);
    pthread_mutex_unlock(&clients_mutex);

    printf("Nowy klient, ID: %d\n", client->id);

    return client;
}

void listen_to_client(client_info *client) {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_handler, client);
}

void *thread_handler(void *arg) {
    client_info *client = (client_info *)arg;
    char buff[1024];

    while (1) {
        ssize_t received = recv(client->fd, buff, 1024, 0);
        if (received > 0) {
            buff[received] = '\0';
            int stop = handle_message(buff, client);
            if (stop == 1) {
                break;
            }
        }
        else {
            break;
        }
    }
    close(client->fd);
    deletenode(client->id);
    pthread_exit(NULL);
}

int handle_message(char msg[], client_info *client) {
    char *cmd;
    cmd = strtok(msg, " ");
    char buff[1024];
    if (strcmp(cmd, "LIST") == 0) {
        int n = 0;
        pthread_mutex_lock(&clients_mutex);
        struct node *temp = head;
        while (temp != NULL) {
            n += snprintf(buff + n, 1024 - n, "%d\n", temp->data.id);
            temp = temp->next;
        }
        printf("wysylam liste: %s\n", buff);
        send(client->fd, buff, strlen(buff), 0);
        pthread_mutex_unlock(&clients_mutex);
    }
    else if (strcmp(cmd, "2ALL") == 0) {
        cmd = strtok(NULL, "");
        time_t curr_time = time(NULL);
        char *time_str = ctime(&curr_time);
        time_str[strlen(time_str) - 1] = '\0';
        int n = snprintf(buff, 1024, "ID: %d:<%s>: %s", client->id, time_str, cmd);
        pthread_mutex_lock(&clients_mutex);
        struct node *temp = head;
        while (temp != NULL) {
            if (temp->data.id != client->id) {
                printf("wysylam %s\n", buff);
                send(temp->data.fd, buff, strlen(buff), 0);
            }
            temp = temp->next;
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    else if (strcmp(cmd, "2ONE") == 0) {
        cmd = strtok(NULL, " ");
        int rec_id = atoi(cmd);
        cmd = strtok(NULL, "");
        time_t curr_time = time(NULL);
        char *time_str = ctime(&curr_time);
        time_str[strlen(time_str) - 1] = '\0';
        int n = snprintf(buff, 1024, "ID: %d:<%s>: %s", client->id, time_str, cmd);
        pthread_mutex_lock(&clients_mutex);
        struct node *temp = head;
        while (temp != NULL) {
            if (temp->data.id == rec_id) {
                send(temp->data.fd, buff, strlen(buff), 0);
                break;
            }
            temp = temp->next;
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    else if (strcmp(cmd, "STOP") == 0) {
        printf("Usuwam klienta o ID: %d\n", client->id);
        deletenode(client->id);
        return 1;
    }
    return 0;

}

void deletenode(int key) {
    struct node *temp = head, *prev;
    if (temp != NULL && temp->data.id == key) {
        head = temp->next;
        return;
    }

    // Find the key to be deleted
    while (temp != NULL && temp->data.id != key) {
        prev = temp;
        temp = temp->next;
    }

    // If the key is not present
    if (temp == NULL) return;

    // Remove the node
    prev->next = temp->next;
}

void insertatbegin(client_info data) {

    //create a link
    struct node *lk = (struct node *)malloc(sizeof(struct node));
    lk->data = data;

    // point it to old first node
    lk->next = head;

    //point first to new first node
    head = lk;
}

void *pinger(void *arg) {
    while (1) {
        pthread_mutex_lock(&clients_mutex);
        struct node *temp = head;
        while (temp != NULL) {
            int error = 0;
            socklen_t len = sizeof(error);
            int retval = getsockopt(temp->data.fd, SOL_SOCKET, SO_ERROR, &error, &len);
            if (retval != 0 || error != 0) {
                close(temp->data.fd);
                printf("Usuwam nieaktywny socket o ID: %d\n", temp->data.id);
                deletenode(temp->data.id);
                break;
            }
            else {
                temp = temp->next;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(5);
    }
    pthread_exit(NULL);
}