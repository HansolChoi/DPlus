#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/time.h>
#include <sys/ioctl.h>
#include <jni.h>
#include <android/log.h>

#define LOG	__android_log_print
#define I	ANDROID_LOG_INFO

#define STORAGE_PATH	"/sdcard/events.txt"

void Replay(const char* EventPath, int* flag, int ScreenWidth, int ScreenHeight)
{
	int event_fd = -1;
	int record_fd = -1;

	struct timeval tdiff;
	struct input_event event;
	int i, outputdev;
	int num_events;
	struct stat statinfo;

	LOG(I, "Replay", "replay start");

	if((event_fd = open(EventPath, O_WRONLY | O_NDELAY)) < 0)
	{
		LOG(I, "Replay", "%s : open error", EventPath);
		close(record_fd);
		return;
	}

	if(stat(STORAGE_PATH, &statinfo) == -1) {
		LOG(I, "Replay", "Couldn't stat input");
		return;
	}

	num_events = statinfo.st_size / (sizeof(struct input_event) + sizeof(int));

	if((record_fd = open(STORAGE_PATH, O_RDONLY)) < 0)
	{
		LOG(I, "Replay", "%s : open error", STORAGE_PATH);
		return;
	}

	ioctl(event_fd, UI_SET_EVBIT, EV_ABS);

	timerclear(&tdiff);

	for(i=0; i< num_events; i++)
	{
		struct timeval now, tevent, tsleep;

		if(read(record_fd, &event, sizeof(event)) != sizeof(event))
		{
			LOG(I, "Replay", "read error");
			break;
		}
		LOG(I, "Replay", "type : %d, code : %d value : %d", event.type, event.code, event.value);

		gettimeofday(&now, NULL);
		if (!timerisset(&tdiff)) {
			timersub(&now, &event.time, &tdiff);
		}

		timeradd(&event.time, &tdiff, &tevent);
		timersub(&tevent, &now, &tsleep);
		if (tsleep.tv_sec > 0 || tsleep.tv_usec > 100)
			select(0, NULL, NULL, NULL, &tsleep);

		event.time = tevent;

		if(write(event_fd, &event, sizeof(event)) != sizeof(event)){
			LOG(I, "Replay", "write error");
			break;
		}
	}

	close(event_fd);
	close(record_fd);
	LOG(I, "Replay", "replay stop");
}
