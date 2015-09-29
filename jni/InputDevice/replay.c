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

int OriginWidth, OriginHeight, DeviceWidth, DeviceHeight;
int EventNum = 0;

void EventPos(struct input_event *event)
{
	if(event->code == ABS_MT_POSITION_X)
	{
			event->value = event->value * DeviceWidth / OriginWidth;
	}
	else if(event->code == ABS_MT_POSITION_Y)
	{
		event->value = event->value * DeviceHeight / OriginHeight;
	}
}

void EventAdd(int event_fd, struct input_event event, int stat)
{
	event.type = 1; event.code = 330; event.value = (stat == 1) ? 1 : 0;

	LOG(I, "EventAdd", "post - type : %d, code : %d, value : %d, num : %d", event.type, event.code, event.value, EventNum);
	if(write(event_fd, &event, sizeof(event)) != sizeof(event)){
		LOG(I, "Replay", "write error");
		return;
	}
}

void Replay(const char* EventPath, int* flag,
		int _OriginWidth, int _OriginHeight, int _DeviceWidth, int _DeviceHeight)
{
	int event_fd = -1;
	int record_fd = -1;

	struct timeval tdiff;
	struct input_event event;
	int i, outputdev;
	int num_events;
	struct stat statinfo;

	int CodeAddStat = 0;

	OriginWidth = _OriginWidth; OriginHeight = _OriginHeight;
	DeviceWidth = _DeviceWidth; DeviceHeight = _DeviceHeight;

	EventNum = 0;

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

	num_events = statinfo.st_size / (sizeof(struct input_event));
	LOG(I, "Replay", "num_events : %d", num_events);

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

		gettimeofday(&now, NULL);
		if (!timerisset(&tdiff)) {
			timersub(&now, &event.time, &tdiff);
		}

		timeradd(&event.time, &tdiff, &tevent);
		timersub(&tevent, &now, &tsleep);
		if (tsleep.tv_sec > 0 || tsleep.tv_usec > 100)
			select(0, NULL, NULL, NULL, &tsleep);

		event.time = tevent;

		// 상위버전의 최소조건 프로토콜로 변환이 필요한가?
		if(event.type == EV_ABS && event.code == ABS_MT_TRACKING_ID && CodeAddStat == 0 ){
			CodeAddStat = (event.value >= 0) ? 1 : 2;
		}
		else if(CodeAddStat > 0 && event.type != 1){
			EventAdd(event_fd, event, CodeAddStat);
			CodeAddStat = 0;
		}else if(CodeAddStat > 0 && event.type == 1){
			CodeAddStat = 0;
		}

		if(event.type == EV_ABS && ( event.code == ABS_MT_POSITION_X || event.code == ABS_MT_POSITION_Y ))
			EventPos(&event);

		LOG(I, "Replay", "post - type : %d, code : %d, value : %d, num : %d", event.type, event.code, event.value, ++EventNum);
		if(write(event_fd, &event, sizeof(event)) != sizeof(event)){
			LOG(I, "Replay", "write error");
			break;
		}
	}

	close(event_fd);
	close(record_fd);
	LOG(I, "Replay", "replay stop");
}
