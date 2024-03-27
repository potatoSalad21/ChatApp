/*
*       SERVER App for the chat program.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 1024


void* handleMsg(void* arg) {
    int client_fd = *((int*) arg);
    
    while (1) {
        char buffer[BUFFER_SIZE];

        printf("receiving\n");
        // receive info and store into the buffer
        ssize_t byteRecv = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (byteRecv > 0) {
            printf("~> %s\n", buffer);
        }
        else {
            printf("[|Closing connection\n");
            break;
        }
    }
	
	free(arg);

    return 0;
}

int main(int argc, char **argv) {
    // create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("||ERROR|| Couldn't create a socket");
        close(sock);
        return 1;
    }

    // configure the socket
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9000);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int option = 1;

    setsockopt(sock, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*) &option, sizeof(option));

    // bind the socket to a port
    if (bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
        perror("||ERROR|| Couldn't bind the socket");
        close(sock);
        return 1;
    } 

    // Listen for the connections
    if (listen(sock, 5) < 0) {
        perror("||ERROR|| smth went wrong in listen()");
        close(sock);
        return 1;
    }

    while (1) {
        // Client info
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
		int* client_fd = malloc(sizeof(int));

        // Accept connections
        *client_fd = accept(sock, (struct sockaddr*) &clientAddr, &clientAddrLen);
        if (*client_fd < 0) {
            perror("ERROR in accept()");
            continue;
        }

        printf("Accepted a connection!\n");

        // Create a separate thread for each connection
        pthread_t threadId;
        pthread_create(&threadId, 0, handleMsg, (void*) client_fd);
        pthread_detach(threadId);

        sleep(1);
    }

    return 0;
}
