/*
*       CLIENT App for the chat program.
*
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024


int main(int argc, char **argv) {
	if (argc < 2 || argc > 3) {
		perror("||ERROR|| Incorrect Amount of Arguments.");
		return 1;
	}
	// Get the server IP from the arguments
	char* servIP = argv[1];

    // Create a TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        perror("||ERROR|| Couldn't create a socket");
        return 1;
    }

    // Socket Address struct
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr)); // zero out the memory
    servAddr.sin_family = AF_INET;          // set the IPv4 family
    servAddr.sin_port = htons(9000);      	// set the port number

    int val = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (val == 0) {
        perror("||ERROR|| Invalid Address");
        return 1;
    }

    // Establish the connection to the server
    int status = connect(sock, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if (status < 0) {
        perror("||ERROR|| Couldn't open a socket connection");
        return 1;
    }

    struct pollfd fds[2] = {
        // user input available
        {
            0,   // 0 == stdin
            POLLIN,
            0
        },
        // message from the server
        {
            sock,
            POLLIN,
            0
        }
    };

    // Polling loop
    while (1) {
        char msg[BUFFER_SIZE] = { 0 };

        poll(fds, 2, 50000);

        if (fds[0].revents & POLLIN) {
            printf("\x1b[1F"); // Move to beginning of previous line
            printf("\x1b[2K"); // Clear entire line
            fgets(msg, BUFFER_SIZE, stdin);
            // Send messages
            if (send(sock, msg, BUFFER_SIZE, 0) < 0) {
                perror("||ERROR|| couldn't send the buffer");
                return 1;
            }
        } else if (fds[1].revents & POLLIN) {
            if (recv(sock, msg, BUFFER_SIZE, 0) > 0) {
                printf("S: %s\n", msg);
            }
        }
    }

    close(sock);
    printf("Connection Ended");

    return 0;
}
