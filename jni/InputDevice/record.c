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

/*
 * 이벤트 디바이스에서 입력을 읽어온다.
 */
void Record(const char* EventPath, int* flag, int ScreenWidth, int ScreenHeight )
{
	int event_fd = -1;
	int record_fd = -1;
	int num;
	struct input_event event;
	char data[200];

	// 녹화용 파일 만들기
	// ** 녹화한 파일에 mode를 설정해주지 않으면, 다른 사용자는 또 루트권한으로 열어야되.
	if((record_fd = open(STORAGE_PATH, O_RDWR | O_CREAT)) < 0)
	{
		LOG(I, "Record", "%s : open error", STORAGE_PATH);
		return;
	}

	if((event_fd = open(EventPath, O_RDONLY)) < 0)
	{
		LOG(I, "Record", "%s : open error", EventPath);
		close(record_fd);
		return;
	}
	//sprintf(data, "%d %d\n", ScreenWidth, ScreenHeight);
	//write(record_fd, data, strlen(data)); // 해상도 입력

	while(*flag)
	{
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
}
