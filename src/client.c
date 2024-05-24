/*
*       CLIENT App for the chat program.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024


int main(int argc, char **argv) {
    if (argc > 2) {
        printf("||Incorrect Amount of Arguments\n");
        return 1;
    }

    char username[32] = "Guest";
    if (argc == 2) {
        if (strlen(argv[1]) > 32) {
            printf("||Username was too long\n");
            return 1;
        }
        strncpy(username, argv[1], 32);
    }

	char* servIP = "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        perror("||ERROR|| Couldn't create a socket");
        return 1;
    }

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9000);

    int val = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (val == 0) {
        perror("||ERROR|| Invalid Address");
        return 1;
    }

    int status = connect(sock, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if (status < 0) {
        perror("||ERROR|| Couldn't open a socket connection");
        return 1;
    }

    // client opts
    send(sock, username, 32, 0);
    int keyRecv = 0;
    char key[16];

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

    while (1) {
        char msg[BUFFER_SIZE] = { 0 };

        poll(fds, 2, 50000);

        if (fds[0].revents & POLLIN) {
            printf("\x1b[1F"); // Move to beginning of previous line
            printf("\x1b[2K"); // Clear entire line
            fgets(msg, BUFFER_SIZE, stdin); // note: fgets appends a newline

            // TODO encrypt the message before sending
            if (send(sock, msg, BUFFER_SIZE, 0) < 0) {
                perror("||ERROR|| couldn't send the buffer");
                return 1;
            }
        } else if (fds[1].revents & POLLIN) {
            if (recv(sock, msg, BUFFER_SIZE, 0) > 0) {
                if (!keyRecv) {
                    strncpy(key, msg, 16);
                    keyRecv = 1;
                } else {
                    printf("%s", msg);
                }
            }
        }
    }

    close(sock);
    printf("Connection Ended");

    return 0;
}
