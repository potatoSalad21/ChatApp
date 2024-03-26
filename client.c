/*
*       CLIENT App for the chat program. 
*
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MSG_LENGTH = 1024;

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

    // Get the user input repeatedly
    while (1 && status == 0) {
        char msg[1024];
        printf("~> ");
        fgets(msg, 1024, stdin); 

        // Send messages
        if (send(sock, msg, sizeof(msg), 0) < 0) {
            perror("||ERROR|| couldn't send the buffer");
            return 1;
        }
    }

    printf("Connection Ended");

    return 0;
}
