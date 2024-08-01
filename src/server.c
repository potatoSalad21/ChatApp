/*
*       SERVER App for the chat program.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 20
#define KEY_LEN 16

static _Atomic unsigned int clientCount = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    struct sockaddr_in addr;
    int sockfd;
    char name[32];
} client_t;

client_t* clientList[MAX_CLIENTS];


void add_client(client_t* cli) {
    pthread_mutex_lock(&client_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clientList[i]) {
            clientList[i] = cli;
            break;
        }
    }

    pthread_mutex_unlock(&client_mutex);
}

void remove_client(int fd) {
    pthread_mutex_lock(&client_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientList[i]) {
            if (clientList[i]->sockfd == fd) {
                clientList[i] = NULL;
            }
        }
    }

    pthread_mutex_unlock(&client_mutex);
}

void send_message(char* msg) {
    pthread_mutex_lock(&client_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientList[i]) {
            if (send(clientList[i]->sockfd, msg, strlen(msg), 0) < 0) {
                perror("||ERROR|| Couldn't send messages");
                continue;
            }
        }
    }

    pthread_mutex_unlock(&client_mutex);
}

void* handleClients(void* arg) {
    client_t* client = (client_t*) arg;
    clientCount++;
    recv(client->sockfd, client->name, 32, 0);

    while (1) {
        char message[BUFFER_SIZE] = { 0 };

        ssize_t byteRecv = recv(client->sockfd, message, BUFFER_SIZE, 0);
        if (byteRecv > 0) {
            char buffer[BUFFER_SIZE + 35] = { 0 };
            strncat(buffer, client->name, BUFFER_SIZE);
            strncat(buffer, ": ", BUFFER_SIZE);
            strncat(buffer, message, BUFFER_SIZE);

            printf("%s\n", buffer);
            send_message(buffer);
        }
        else {
            printf("[-] Closing connection [-]\n");
            break;
        }
    }

    remove_client(client->sockfd);
    close(client->sockfd);
	free(client);
    clientCount--;
    return 0;
}

int main(int argc, char **argv) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("||ERROR|| Couldn't create a socket");
        close(sock);
        return 1;
    }

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9000);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int option = 1;

    setsockopt(sock, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*) &option, sizeof(option));

    if (bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
        perror("||ERROR|| Couldn't bind the socket");
        close(sock);
        return 1;
    }

    if (listen(sock, 5) < 0) {
        perror("||ERROR|| in listen()");
        close(sock);
        return 1;
    }

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        int newClient = accept(sock, (struct sockaddr*) &clientAddr, &clientAddrLen);
        if (newClient < 0) {
            perror("ERROR in accept()");
            close(newClient);
            continue;
        }
        if (clientCount > MAX_CLIENTS) {
            printf("Maximum number of clients exceeded: Rejecting\n");
            close(newClient);
            continue;
        }

        client_t* client = malloc(sizeof(client_t));
        client->addr = clientAddr;
        client->sockfd = newClient;

        printf("[+] Accepted a connection! [+]\n");
        add_client(client);

        pthread_t threadId;
        pthread_create(&threadId, 0, handleClients, (void*) client);
        pthread_detach(threadId);

        sleep(1);
    }

    return 0;
}
