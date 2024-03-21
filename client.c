/*
*       CLIENT App for the chat program. 
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
	if (argc < 2 || argc > 3) 
  {
		printf("||ERROR|| Incorrect Amount of Arguments.\n");
		return 1;
	}
	// Get the server IP from the arguments
	char* servIP = argv[1];
	
	// Create a TCP socket
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) 
  {
		printf("||ERROR|| Couldn't create a socket\n");
		return 1;
	}
	
	// Socket Address struct
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr)); // zero out the memory
	servAddr.sin_family = AF_INET;          // set the IPv4 family
	servAddr.sin_port = htons(6969);      	// set the port number

	int val = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
	if (val == 0) 
  {
		printf("||ERROR|| Invalid Address\n");
		return 1;
	}

	// Establish the connection to the server
	if (connect(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) 
  {
		printf("||ERROR|| Couldn't open a socket connection\n");
		return 1;
	}

	char msg[1024];

	strcpy(msg, "hello world");

	// Send messages
	if (send(sock, msg, sizeof(msg), 0) < 0) 
  {
		printf("||ERROR|| couldn't send the buffer\n");
		return 1;
	}

  // TODO make way to accept user input (messages)

	char buffer[1024];
  if (recv(sock, buffer, sizeof(buffer), 0) < 0) 
  {
    printf("||ERROR|| Tcp error\n");
    return 1;
  }
	
  printf("Client Ended Successfully\n");
  return 0;
}
