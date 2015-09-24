#ifndef __LOGCAT_H
#define __LOGCAT_H

pid_t pid;		//child process pid
int logcat_fd;	// logcat socket file descriptor

void LogcatExcute();
void LogcatStop();
void _waitpid();
int LogcatStdoutChange(int socket_logcat_fd);
int StdoutChange(char* path);

#endif
