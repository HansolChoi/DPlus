#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <jni.h>

#define LOG	__android_log_print
#define I	ANDROID_LOG_INFO

#define STORAGE_PATH	"/sdcard/events.txt"

int event_fd = -1;
int record_fd = -1;
/*
 * 이벤트 디바이스에서 입력을 읽어온다.
 */

void Record(const char* EventPath, int* flag, int ScreenWidth, int ScreenHeight, int* loop)
{
	int num;
	struct input_event event;
	char data[200];

	// 녹화용 파일 만들기
	system("su rm -f /sdcard/events.txt");

	if((record_fd = creat(STORAGE_PATH, 0777)) < 0)
	{
		LOG(I, "Record", "%s : creat error", STORAGE_PATH);
		return;
	}
	LOG(I, "Record", "%s : creat success!", STORAGE_PATH);

	if((event_fd = open(EventPath, O_RDONLY)) < 0)
	{
		LOG(I, "Record", "%s : open error", EventPath);
		close(record_fd);
		return;
	}

	LOG(I, "Record", "이벤트 읽을 준비 완료!");
	while(*flag)
	{
		*loop = 1;
		num = read(event_fd, &event, (sizeof(event)));
		LOG(I, "Record", "type : %d, code : %d, value : %d", event.type, event.code, event.value);

		if(num != sizeof(event))
		{
			LOG(I, "Record", "read error");
			break;
		}

		if( write(record_fd, &event, sizeof(event)) != sizeof(event)){
			LOG(I, "Record", "write error");
			break;
		}
	}
	LOG(I, "Record", "Record exit");
	close(event_fd);
	close(record_fd);
	*loop = 0;
}
