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
#include <stdbool.h>

#define DEFAULT_PORT 8080
#define DEFAULT_IP "127.0.0.1"
#define USERNAME_LEN 50
#define BUFF_LEN 1024

typedef struct client {
    struct sockaddr_in address;
    char username[USERNAME_LEN];
    struct client *next;
    int id;
}client;

client client_list;

int connected_clients = 0;
char request_buff[BUFF_LEN];
char response_buff[BUFF_LEN + USERNAME_LEN];

char sender_name[USERNAME_LEN];
int sender_id;

int sockfd;

bool cmp_client(struct sockaddr_in client1, struct sockaddr_in client2) {
    if (strncmp((char *)&client1.sin_addr.s_addr, (char *)&client2.sin_addr.s_addr, sizeof(unsigned long)) == 0) {
        if (strncmp((char *)&client1.sin_port, (char *)&client2.sin_port, sizeof(unsigned short)) == 0) {
            if (strncmp((char *)&client1.sin_family, (char *)&client2.sin_family, sizeof(unsigned short)) == 0) {
                return true;
            }
        }
    }
    return false;
}

bool is_connected(struct sockaddr_in new_client) {
    client *tmp = &client_list;

    while (tmp != NULL) {
        if (cmp_client(tmp->address, new_client) == true) {
            strncpy(sender_name, tmp->username, USERNAME_LEN);
            sender_id = tmp->id;
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

void dm(struct sockaddr_in sender, int id) {
    client *tmp = &client_list;

    while (tmp != NULL) {
        if (tmp->id == id) {
            if (sendto(sockfd, response_buff, strlen(response_buff), 0, (struct sockaddr *)&tmp->address, sizeof(tmp->address)) == -1) {
                perror("sendto");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            break;
        }
        tmp = tmp->next;
    }
}

void dm_client_list(struct sockaddr_in sender) {
    client *tmp = &client_list;
    int n = 0;
    while (tmp != NULL) {
        n += snprintf(response_buff + n, 1024 - n, "%d: %s\n", tmp->id, tmp->username);
        tmp = tmp->next;
    }
    if (sendto(sockfd, response_buff, strlen(response_buff), 0, (struct sockaddr *)&sender, sizeof(sender)) == -1) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

void broadcast(struct sockaddr_in sender) {
    client *tmp = &client_list;

    while (tmp != NULL) {
        if (cmp_client(sender, tmp->address) == false && tmp->id != 0) {
            if (sendto(sockfd, response_buff, strlen(response_buff), 0, (struct sockaddr *)&tmp->address, sizeof(tmp->address)) == -1) {
                perror("sendto");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
        }

        tmp = tmp->next;
    }
}

bool connect_client(struct sockaddr_in new_client, char *username) {
    client *tmp = &client_list;
    tmp = &client_list;

    while (tmp->next != NULL) {
        tmp = tmp->next;
    }

    tmp->next = malloc(sizeof(client));
    tmp = tmp->next;
    connected_clients++;
    tmp->id = connected_clients;
    tmp->address = new_client;
    strncpy(tmp->username, username, USERNAME_LEN);
    strncpy(sender_name, username, USERNAME_LEN);
    tmp->next = NULL;
    sender_id = connected_clients;
    printf("New client connected - ID: %d, USERNAME: %s\n", tmp->id, tmp->username);
    return true;
}

bool disconnect_client(int id) {
    client *prev = NULL;
    client *curr = &client_list;

    while (curr != NULL) {
        if (curr->id == id) {
            prev->next = curr->next;
            free(curr);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

int handle_message(struct sockaddr_in sender) {
    char *cmd;
    cmd = strtok(request_buff, " \n");
    if (strcmp(cmd, "LIST") == 0) {
        printf("LIST printing\n");
        dm_client_list(sender);
    }
    else if (strcmp(cmd, "2ALL") == 0) {
        printf("BROADCAST\n");
        cmd = strtok(NULL, "");
        time_t curr_time = time(NULL);
        char *time_str = ctime(&curr_time);
        time_str[strlen(time_str) - 1] = '\0';
        snprintf(response_buff, 1024, "%d:%s<%s>: %s", sender_id, sender_name, time_str, cmd);
        broadcast(sender);
    }
    else if (strcmp(cmd, "2ONE") == 0) {
        printf("DM\n");
        cmd = strtok(NULL, " ");
        int rec_id = atoi(cmd);
        cmd = strtok(NULL, "");
        time_t curr_time = time(NULL);
        char *time_str = ctime(&curr_time);
        time_str[strlen(time_str) - 1] = '\0';
        snprintf(response_buff, 1024, "%d:%s<%s>: %s", sender_id, sender_name, time_str, cmd);
        dm(sender, rec_id);
    }
    else if (strcmp(cmd, "STOP") == 0) {
        printf("STOP request\n");
        if (!disconnect_client(sender_id)) {
            printf("Unsuccessful disconnecting\n");
        }
    }
    return 0;

}

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

    int nbytes;
    int address_size = sizeof(struct sockaddr_in);

    struct sockaddr_in server_addr;
    struct sockaddr_in sender_addr;

    bzero(request_buff, BUFF_LEN);
    bzero(response_buff, BUFF_LEN + USERNAME_LEN);


    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        close(sockfd);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    int s = inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
    if (s <= 0) {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening at %s:%d\n", ip, port);


    while (1) {
        bzero(response_buff, BUFF_LEN + USERNAME_LEN);
        bzero(request_buff, BUFF_LEN);

        if ((nbytes = recvfrom(sockfd, request_buff, BUFF_LEN - 1, 0, (struct sockaddr *)&sender_addr, (unsigned int *)&address_size)) == -1) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }


        request_buff[nbytes - 1] = '\0';
        if (is_connected(sender_addr)) {
            handle_message(sender_addr);
        }
        else {
            if (connect_client(sender_addr, request_buff) == true) {
                snprintf(response_buff, 1024, "Hi %s, your ID is: %d\n", sender_name, sender_id);
                if (sendto(sockfd, response_buff, strlen(response_buff), 0, (struct sockaddr *)&sender_addr, sizeof(sender_addr)) == -1) {
                    perror("sendto");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}

