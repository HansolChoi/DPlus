#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "socket.h"

typedef struct Socket
{
	int sockfd;
	struct sockaddr_in svr_addr;
}Socket;

Socket SocketList[SOCKETMAX];
char server_addr[30];

int CreateSocket(int ip, int port){
	int CreateSockfd;
	int SocketPortNumber = port - PORT_NUMBER;
	struct hostent *pHostEnt;

	strcat(server_addr, ip);
	SocketList[SocketPortNumber].sockfd = CreateSockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(CreateSockfd < 0)
	{
		LOG(ANDROID_LOG_DEBUG, "DEBUGGING", "Socket Creation Error: socketfd = %d", SocketList[SocketPortNumber].sockfd);
	 	return 0;
	}

	memset(&SocketList[SocketPortNumber].svr_addr, 0, sizeof(struct sockaddr_in));

	SocketList[SocketPortNumber].svr_addr.sin_family = AF_INET;
	SocketList[SocketPortNumber].svr_addr.sin_port = htons(port);

	pHostEnt = gethostbyname(server_addr);
	SocketList[SocketPortNumber].svr_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)pHostEnt->h_addr)));

	if(connect(CreateSockfd, (struct sockaddr*)&SocketList[SocketPortNumber].svr_addr, sizeof(SocketList[SocketPortNumber].svr_addr)) < 0)
	{
		LOG(ANDROID_LOG_DEBUG, "DEBUGGING", "Can't connect to the Server.");
		return 0;
	}

	CurrentSocketNumber++;
	LOG(ANDROID_LOG_DEBUG, "DEBUGGING", "Connect to the Server successfully.");
	return 1;
}

int SocketSends(int *data, int len, int port){
	int SocketPortNumber = port - PORT_NUMBER;

	if (write(SocketList[SocketPortNumber].sockfd, str, len) < 0) {
	   LOG(ANDROID_LOG_DEBUG, "DEBUGGING", "Writing Error.");
	   fsync(SocketList[SocketPortNumber].sockfd);
	   return 0;
	 }
	 return 1;
}

int SocketReceives(int* data, int len, int port){
	int retval;
	int SocketPortNumber = port - PORT_NUMBER;

	retval = recv(SocketList[SocketPortNumber].sockfd, data, len, 0);
	if (retval == -1) {
	   LOG(ANDROID_LOG_DEBUG, "DEBUGGING", "Receive Error.");
	}
	LOG(ANDROID_LOG_DEBUG, "DEBUGGING", "%d", data[0]);
}

void SocketStop(int port){
	int SocketNumber = port - PORT_NUMBER;

	if(SocketList[SocketNumber].port == port)
		close(SocketList[SocketNumber].sockfd);
}
