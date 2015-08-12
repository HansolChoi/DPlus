#ifndef __SOCKET_H
#define __SOCKET_H

#define __android_log_print LOG

#define PORT_NUMBER	5000
#define SOCKETMAX	30
#define SOCKET_SIZE	1024

int CreateSocket(const char *ip, int port);
int SocketSends(int *data, int len, int port);
int SocketReceives(int* data, int len, int port);
void SocketStop(int port);

#endif
