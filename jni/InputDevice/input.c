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

#include <linux/fb.h>
#include <linux/kd.h>

#include "input.h"

#define LOG	__android_log_print
#define STORAGE_PATH	"/sdcard/InputRecord.txt"

jclass sh_cls;
jmethodID func;
jobject sh_obj;
int decimal[10] = {1, };

/*
 * 터치스크린 장치와 이벤트 장치를 매칭시킨다.
 */
int MatchingEventDevice()
{
	FILE *MatchingFile = NULL;
	char Line[300];
	int idx, res = -1;

	MatchingFile = fopen("/proc/bus/input/devices", "r");

	if(MatchingFile == NULL)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "MatchingFile opennig fail.");
		return;
	}

	while(!feof(MatchingFile))
	{
		fgets(Line, sizeof(Line), MatchingFile);
		if(strcmp(Line, "N: Name=\"sec_touchscreen\"\n") == 0)
		{
			for(idx=0; idx < 4; idx++)
				fgets(Line, sizeof(Line), MatchingFile);

			LOG(ANDROID_LOG_INFO, "NATIVE", "%s", Line);
			res = Line[17] - '0'; break;
		}
	}

	fclose(MatchingFile);
	return res;
}

/*
 * 이벤트 장치의 권한을 변경한다.
 */
void EvenvDeviceAuthorityChange(JNIEnv* env, const char* file)
{
	char chmod[100] = "chmod 777 ";

	JavaShellEnv(env);

	JavaShell(env, "su"); // 루트권한 획득
	strcat(chmod, file);
	JavaShell(env, chmod); // 권한 변경
}

/*
 * 쉘 환경설정
 */
void JavaShellEnv(JNIEnv* env)
{
	// 클래스  찾기
	sh_cls = (*env)->FindClass(env, "Shell/ShellReceiver");
	if(sh_cls == NULL){
		LOG(ANDROID_LOG_INFO, "NATIVE", "클래스 검색 실패");
		return;
	}

	// 생성자 찾기
	func = (*env)->GetMethodID(env, sh_cls, "<init>", "()V");
	if(func == NULL){
		LOG(ANDROID_LOG_INFO, "NATIVE", "생성자를 못찾음");
		return;
	}

	// 객체 생성
	sh_obj = (*env)->NewObject(env, sh_cls, func);
	if(sh_obj == NULL){
		LOG(ANDROID_LOG_INFO, "NATIVE", "객체 생성 실패");
		return;
	}

	// 객체의 메소드 호출
	func = (*env)->GetMethodID(env, sh_cls, "ShellCmd", "(Ljava/lang/String;)V");
	if(func == NULL){
		LOG(ANDROID_LOG_INFO, "NATIVE", "객체 메소드 찾기 실패");
		return;
	}
}

/*
 * 쉘 명령 입력
 */
void JavaShell(JNIEnv* env, const char* param)
{
	jstring cmd;

	// 명령어 jstring으로 변환
	cmd = (*env)->NewStringUTF(env, param);
	(*env)->CallVoidMethod(env, sh_obj, func, cmd);
}

/*
 *  해상도 : [0] : height, [1] : width
 *  접  촉  : [0] : 접촉여부, [1] = [2] = -1
 *  움직임 : [0] : 좌표, [1] : 다음까지 걸린 초, [2] : 다음까지 걸린 마이크로 초
 */
void ReadLine(FILE* file, int* value)
{
	char Line[200];
	int i = 0, j = 0, k = 0;
	int len = 0;

	fgets(Line, sizeof(Line), file);
	LOG(ANDROID_LOG_INFO, "ReadLine", "%s", Line);
	if(Line[0] == 'R' || Line[0] == 'P'){
		value[2] = Line[0]; value[0] = value[1] = -1;
		return;
	}

	for(i=0; Line[i] != '\0'; i++) len++;

	for(i = len -1; i >= 0; i--)
	{
		if(Line[i] == ' ')
		{
			j++; k = 0; continue;
		}
		if(Line[i] >= '0' && Line[i] <= '9')
			value[j] += (Line[i] - '0') * decimal[k++];
	}
}

/*
 * 입력 디바이스의 입력을 녹화한다.
 */
void write_file(int fd, const char mark, struct input_event event_buf)
{
	char data[200];
	char LogPrint[200];
	static long pre_xsec = 0, pre_xusec = 0, pre_ysec = 0, pre_yusec = 0;
	int term_sec, term_usec;

	if(mark == 'X'){
		term_sec = event_buf.time.tv_sec - pre_xsec;
		term_usec = event_buf.time.tv_usec - pre_xusec;

		pre_xsec = event_buf.time.tv_sec;
		pre_xusec = event_buf.time.tv_usec;

	}else{
		term_sec = event_buf.time.tv_sec - pre_ysec;
		term_usec = event_buf.time.tv_usec - pre_yusec;

		pre_ysec = event_buf.time.tv_sec;
		pre_yusec = event_buf.time.tv_usec;
	}

	if(term_sec > 100000) term_sec = term_usec = 0;
	if(term_usec < 0){
		term_sec--; term_usec = 1000000 - term_usec;
	}
	sprintf(data, "%c%d %d %d\n", mark,event_buf.value, term_sec, term_usec);
	write(fd, data, strlen(data));

	sprintf(LogPrint, "%c position: %d, Sec : %d, Usec : %d", mark, event_buf.value, term_sec, term_usec);
	LOG(ANDROID_LOG_INFO, "TOUCH", "%s", LogPrint);
}

/*
 * 녹화된 내용을 재생한다.
 */
void PlayingLoop(const char* file, int* flag, int ScreenWidth, int ScreenHeight )
{
	int event_fd = -1;
	FILE *RecordFile = NULL;
	struct input_event event;
	char record_file_name[100] = STORAGE_PATH;
	int Resolution[2] = {0, 0};
	int data[3];
	int i, delay, udelay;

	LOG(ANDROID_LOG_INFO, "PlayingLoop", "Play start");
	// 녹화한 파일 열기
	RecordFile = fopen(record_file_name, "r");
	if(RecordFile == NULL)
	{
		LOG(ANDROID_LOG_INFO, "PlayingLoop", "RecordingFile opennig fail.");
		return;
	}

	if((event_fd = open(file, O_WRONLY)) < 0)
	{
		LOG(ANDROID_LOG_INFO, "PlayingLoop", "%s : open error", file);
		fclose(RecordFile);
		return;
	}

	for (i = 1; i < 10; i++) decimal[i] = decimal[i - 1] * 10;

	ReadLine(RecordFile, Resolution);
	LOG(ANDROID_LOG_INFO, "PlayingLoop", "%d %d", Resolution[1], Resolution[0]);

	while(!feof(RecordFile))
	{
		ReadLine(RecordFile, data);
		LOG(ANDROID_LOG_INFO, "PlayingLoop", "%d %d %d", data[2], data[1], data[0]);
		if(data[1] = -1) // 버튼을 누르거나 뗀 경우
		{
			event.type = EV_KEY;
			event.code = 330;
			event.value = (data[2] == 'P') ? 1 : 0;
			delay = udelay = 0;
			write(event_fd, &event, sizeof(struct input_event));
		}else{ 			// 좌표일 때.
			event.type = EV_ABS;
			event.code = ABS_MT_POSITION_X;
			event.value = data[2];
			write(event_fd, &event, sizeof(struct input_event));

			ReadLine(RecordFile, data);
			LOG(ANDROID_LOG_INFO, "PlayingLoop", "%d %d %d", data[2], data[1], data[0]);

			event.type = EV_ABS;
			event.code = ABS_MT_POSITION_Y;
			event.value = data[2];
			delay = data[1]; udelay = data[0];

			write(event_fd, &event, sizeof(struct input_event));
		}
		event.type = EV_SYN;
		event.code = 0;
		event.value = 0;
		write(event_fd, &event, sizeof(struct input_event));

		sleep(delay); usleep(udelay);
	}

	close(event_fd);
	fclose(RecordFile);
	LOG(ANDROID_LOG_INFO, "PlayingLoop", "play stop");
}

/*
 * 이벤트 디바이스에서 입력을 읽어온다.
 */
void ReadingLoop(const char* event, int* flag, int ScreenWidth, int ScreenHeight )
{
	int event_fd = -1;
	int record_fd = -1;
	int i;
	size_t	read_bytes;
	struct input_event event_buf[EVENT_BUF_NUM];
	char record_file_name[100] = STORAGE_PATH;
	char data[200];

	// 녹화용 파일 만들기
	// ** 녹화한 파일에 mode를 설정해주지 않으면, 다른 사용자는 또 루트권한으로 열어야되.
	if((record_fd = open(record_file_name, O_RDWR | O_CREAT)) < 0)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "%s : open error", STORAGE_PATH);
		return;
	}

	if((event_fd = open(event, O_RDONLY)) < 0)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "%s : open error", event);
		close(record_fd);
		return;
	}
	sprintf(data, "%d %d\n", ScreenWidth, ScreenHeight);
	write(record_fd, data, strlen(data)); // 해상도 입력

	while(*flag)
	{
		read_bytes = read(event_fd, event_buf, (sizeof(struct input_event) * EVENT_BUF_NUM));
		if(read_bytes < sizeof(struct input_event))
		{
			LOG(ANDROID_LOG_INFO, "NATIVE", "%s : read error", event);
			break;
		}

		for(i=0; i<(read_bytes/sizeof(struct input_event)); i++)
		{
			switch(event_buf[i].type)
			{
			case EV_SYN:
				LOG(ANDROID_LOG_INFO, "TOUCH", "===========================================");
				break;
			case EV_KEY:
				switch(event_buf[i].value)
				{
				case 1:
					LOG(ANDROID_LOG_INFO, "TOUCH", ": pressed");
					write(record_fd, "P\n",2);
					break;
				case 0:
					LOG(ANDROID_LOG_INFO, "TOUCH", ": released");
					write(record_fd, "R\n", 2);
					break;
				}
				break;
			case EV_ABS:
				switch(event_buf[i].code)
				{
				case ABS_MT_POSITION_X:
					write_file(record_fd, 'X', event_buf[i]);
					break;

				case ABS_MT_POSITION_Y:
					write_file(record_fd, 'Y', event_buf[i]);
					break;
				}
				break;
			}
		}
	}
	close(event_fd);
	close(record_fd);
}
