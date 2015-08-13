#ifndef __SOCKET_H
#define __SOCKET_H

#define LOG	__android_log_print

#define PORT_NUMBER	5000
#define SOCKETMAX	30
#define SOCKET_BUF	1024

int CreateSocket(JNIEnv *env, jobject thiz, jint port);
int SocketSend(char *data, int len, int port);
char* SocketReceive(int port);
void SocketStop(JNIEnv *env, jobject thiz, jint port);

#endif
