#ifndef __SOCKET_H
#define __SOCKET_H

#define LOG	__android_log_print

#define PORT_NUMBER	5000
#define SOCKETMAX	30
#define SOCKET_BUF	1024

int CreateSocket(char* ip, int port);
int SocketSend(char *data, int len, int port);
char* SocketReceive(int port);
void SocketStop(int port);

#endif
