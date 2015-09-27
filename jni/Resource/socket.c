#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

//#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "resource.h"
#include "socket.h"
#include "logcat.h"

int CreateSocket(Socket* SocketObj, int port);
void SocketStop(Socket* SocketObj);
int SocketSends(Socket* SocketObj, float *data, int len);

int SocketsInit(const char* ip)
{
	memset(server_addr,0,sizeof(server_addr));
	strcat(server_addr, ip);
	 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "copy ip : %s", server_addr);

	if(CreateSocket(&socket_system_resource, PORT2) == 0)
		return 0;
	if(CreateSocket(&socket_logcat, PORT4) == 0)
		return 0;
	LogcatStdoutChange(socket_logcat.sockfd);
	LogcatExcute();
}

int SocketsDestroy()
{
	LogcatStop();
	SocketStop(&socket_system_resource);
	SocketStop(&socket_logcat);
}

void SocketsTransfer()
{
	int static count = 0; count++;
	CpuUsage(); MemoryUsage(); BatteryUsage();

	SocketSends(&socket_system_resource, resource, 10);
}

int CreateSocket(Socket* SocketObj, int port){
	 struct hostent *pHostEnt;

	 SocketObj->sockfd = socket(PF_INET, SOCK_STREAM, 0);
	 if(SocketObj->sockfd < 0) {
		 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Socket Creation Error: socketfd = %d", SocketObj->sockfd);
	 	return 0;
	 }else{
		 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Socket Created Successfully.");
	 }

	 memset(&SocketObj->svr_addr, 0, sizeof(struct sockaddr_in));
	 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Socket Created 1.");

	 SocketObj->svr_addr.sin_family = AF_INET;
	 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Socket Created 2.");

	 SocketObj->svr_addr.sin_port = htons(port);

	 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Socket Created 3.");

	 pHostEnt = gethostbyname(server_addr);
	 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Socket Created 4.");
	 if(pHostEnt == NULL){
		 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "pHostEnt is null.");
		 return 0;
	 }
	 SocketObj->svr_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)pHostEnt->h_addr)));

	 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "connect prepare...");

	 // Change to ConnectWait() that apply timeout.
	 /*if(connect(SocketObj->sockfd, (struct sockaddr*)&SocketObj->svr_addr, sizeof(SocketObj->svr_addr)) < 0){
		  __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Can't connect to the Server.");
		  return 0;
	 } else {
		  __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Connect to the Server successfully.");
		  return 1;
	 }*/

	 // TimeOut code add.
	 // timeout = 5sec
	 if (ConnectWait(SocketObj->sockfd, (struct sockaddr*)&SocketObj->svr_addr, sizeof(SocketObj->svr_addr), 5) < 0) {
		 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Can't connect to the Server.");
		 return 0;
	 }else{
		 __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Connect to the Server successfully.");
		 return 1;
	 }
}

int SocketSends(Socket* SocketObj, float *data, int len){
	char str[200];
	char num[10];
	int i;

	memset(str, 0, sizeof(str));

	/* Resource's communication method is promised with server.
	*  CPU max value + ';' + CPU user mode percentage + ';' +
	*  Memory total value + ';' + Memory used KB
	*/

	strcat(str, "100;");
	sprintf(num, "%.2f;",data[1]); strcat(str, num);
	sprintf(num, "%d;", (int) data[4]); strcat(str, num);
	sprintf(num, "%d;", (int) data[5]); strcat(str, num);
	strcat(str, "100;");
	sprintf(num, "%d;", (int) data[6]); strcat(str, num);

	if (write(SocketObj->sockfd, str, 200) < 0) {
	   //__android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Writing Error.");
	   fsync(SocketObj->sockfd);
	   return 0;
	 }
	 //__android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Send to message successfully.");
	 return 1;
}

int SocketReceives(Socket* SocketObj){
	int retval;
	retval = recv(SocketObj->sockfd, SocketObj->buf, BUFSIZE, 0);
	if (retval == -1) {
	   __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Receive Error.");
	}
	__android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "%s",SocketObj->buf);

}

void SocketStop(Socket* SocketObj){
	__android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "native socket stop.");
	close(SocketObj->sockfd);
}

int ConnectWait(int sockfd, struct sockaddr *saddr, int addrsize, int sec)
{
    int newSockStat;
    int orgSockStat;
    int res, n;
    fd_set  rset, wset;
    struct timeval tval;

    int error = 0;
    int esize;

    if ( (newSockStat = fcntl(sockfd, F_GETFL, NULL)) < 0 )
    {
        perror("F_GETFL error");
        return -1;
    }

    orgSockStat = newSockStat;
    newSockStat |= O_NONBLOCK;

    // It makes sockfd  to Non blocking state
    if(fcntl(sockfd, F_SETFL, newSockStat) < 0)
    {
        perror("F_SETLF error");
        return -1;
    }

    // Connect wait.
    // This state is Non blocking. so direct return.
    if((res = connect(sockfd, saddr, addrsize)) < 0)
    {
        if (errno != EINPROGRESS)
            return -1;
    }

    printf("RES : %d\n", res);
    // If direct connection success, socket state give back original state.
    if (res == 0)
    {
        printf("Connect Success\n");
        fcntl(sockfd, F_SETFL, orgSockStat);
        return 1;
    }

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;

    tval.tv_sec        = sec;
    tval.tv_usec    = 0;

    if ( (n = select(sockfd+1, &rset, &wset, NULL, NULL)) == 0)
    {
        // Timeout
        errno = ETIMEDOUT;
        return -1;
    }

    // Check writing or reading.
    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset) )
    {
        printf("Read data\n");
        esize = sizeof(int);
        if ((n = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&esize)) < 0)
            return -1;
    }
    else
    {
        perror("Socket Not Set");
        return -1;
    }


    fcntl(sockfd, F_SETFL, orgSockStat);
    if(error)
    {
        errno = error;
        perror("Socket");
        return -1;
    }

    return 1;
}
