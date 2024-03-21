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


int main(int argc, char **argv)
{
  // TODO make a thread receiving messages
  
  // configure the socket
  struct sockaddr_in servAddr;
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(6000);
  servAddr.sin_addr.s_addr = INADR_ANY;

  if (bind()) 
  







  // TODO send messages over to all clients


  return 0;
}
