#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "logcat.h"

void _waitpid()
{
	int state;
	pid_t child;

	// waitpid function is called by child process.
	// when child process die, that occur SIGCHID signal to parent process.
	// so, parent process receive SIGCHID signal through waitpid().
	child = waitpid(-1, &state, WNOHANG);
	if(child != 0){
		__android_log_print(ANDROID_LOG_INFO, "_waitpid", "%d", child);
	}
}

void LogcatExcute(){

	system("/system/bin/logcat -c");

	if((pid = fork()) < 0){
		__android_log_print(ANDROID_LOG_INFO, "NATIVE", "fork1 error");
		return;
	}else if(pid == 0){
		system("su cat /system/bin/logcat -v time");
		__android_log_print(ANDROID_LOG_INFO, "Child Process", "Child Process kill");
		return;
	}
}

void LogcatStop(){
	kill((int)pid, SIGKILL);
}

int LogcatStdoutChange(int socket_logcat_fd){
	int return_fd = 0;
	dup2( socket_logcat_fd, STDOUT_FILENO );
	close(socket_logcat_fd);
	return 1;
}
