/*
*       SERVER App for the chat program.
*
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>


void* handleMsg(void* client_fd)
{
  // receive info and store into the buffer
  char buffer[1024];
  ssize_t byteRecv = recv(client_fd, buffer, 1024, 0);

  if (byteRecv > 0) {
    printf("~> %s\n", buffer);
  }
}

int main(int argc, char **argv)
{
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

  // bind the socket to a port
  if (bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
    perror("||ERROR|| Couldn't bind the socket");
    close(sock);
    return 1;
  } 
  
  // listen for the connections
  if (listen(sock, 10) < 0) {
    perror("||ERROR|| smth went wrong in listen()");
    close(sock);
    return 1;
  }

  // loop for accepting the connections and handling them on seperate threads
  while (1) {
    // accept client connections
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int client_fd = accept(sock, (struct sockaddr*) &clientAddr, &clientAddrLen);
    if (client_fd < 0) {
      perror("ERROR in accept()");
      close(sock);
      return 1;
    }

    printf("Accepted a connection!\n");

    pthread_t threadId;
    pthread_create(&threadId, 0, handleMsg, (void*) client_fd);
    pthread_detach(threadId);
  }

  printf("The chat ended. Closing the socket.\n");
  close(sock);

  return 0;
}
