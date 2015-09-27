#ifndef __SOCKET_H
#define __SOCKET_H

#include <arpa/inet.h>

#define BUFSIZE 	1024

#define PORT1 		5555                    // Port Number
#define PORT2 		5556
#define PORT3 		5557
#define PORT4 		5558

char server_addr[30];

typedef struct Socket{
	char buf[BUFSIZE];
	int sockfd;
	struct sockaddr_in svr_addr;
}Socket;

Socket socket_system_resource;
Socket socket_logcat;

int SocketsInit(const char* ip);
int SocketsDestroy();
void SocketsTransfer();

#endif
