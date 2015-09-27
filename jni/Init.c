#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#include "InputDevice/EnvSetting.h"
#include "InputDevice/record.h"
#include "InputDevice/replay.h"

#include "Resource/logcat.h"
#include "Resource/resource.h"
#include "Resource/socket.h"

#define PAKAGE_CLASS_NAME	"NativeService/NativeService"
#define LOG	__android_log_print
#define I	ANDROID_LOG_INFO

JavaVM *glpVM = NULL;
JNIEnv* env = NULL;
jclass cls;

int Recording_thr_id;
int Playing_thr_id;
int Socket_thr_id;

pthread_t Recording_thread;
pthread_t Playing_thread;
pthread_t Socket_thread;

int RecordingThreadFlag;
int PlayingThreadFlag;
int SocketThreadFlag;

char EventFileName[30];
int DeviceWidth, DeviceHeight;
int OriginWidth, OriginHeight;
int record_loop = 0;

void *Socket_function(void *data)
{
	while(SocketThreadFlag)
	{
		_waitpid();
		SocketsTransfer();
		usleep(200000);
	}
}

void *Recording_function(void *data)
{
	LOG(I, "Recording_function", "스레드 시작해.");
	Record(EventFileName, &RecordingThreadFlag, DeviceWidth, DeviceHeight, &record_loop);
}

void *Playing_function(void *data)
{
	Replay(EventFileName, &RecordingThreadFlag, OriginWidth, OriginHeight, DeviceWidth, DeviceHeight);
}

void RecordStart(JNIEnv *R_Env, jobject thiz, jint Width, jint Height)
{
	RecordingThreadFlag = 1;
	int  EventNumber;

	/**/
	EventNumber = MatchingEventDevice();
	sprintf(EventFileName, "/dev/input/event%d", EventNumber);
	LOG(I, "SocketsInitCall", "%s", EventFileName);
	EventDeviceAuthorityChange(env, EventFileName);
	/* */

	DeviceWidth = Width; DeviceHeight = Height;

	Recording_thr_id = pthread_create(&Recording_thread, NULL, Recording_function, (void *)NULL);
	if(Recording_thr_id < 0)
		LOG(I, "RecordStart", "Create thread fail.\n");
}

void RecordStop()
{
	RecordingThreadFlag = 0;

	// 파일이 닫힐 때까지 기다린다.
	while(record_loop != 0){
		usleep(200000);
	}
}

void PlayInputTest(JNIEnv *R_Env, jobject thiz, jint Origin_Width, jint Origin_Height, jint Device_Width, jint Device_Height)
{
	PlayingThreadFlag = 1;
	int  EventNumber;

	LOG(I, "PlayInputTest", "Play!~~!\n");
	OriginWidth = Origin_Width, OriginHeight = Origin_Height;
	DeviceWidth = Device_Width; DeviceHeight = Device_Height;

	/* */
	EventNumber = MatchingEventDevice();
	sprintf(EventFileName, "/dev/input/event%d", EventNumber);
	LOG(I, "SocketsInitCall", "%s", EventFileName);
	EventDeviceAuthorityChange(env, EventFileName);
	/* */

	Playing_thr_id = pthread_create(&Playing_thread, NULL, Playing_function, (void *)NULL);
	if(Playing_thr_id < 0)
		LOG(I, "PlayInputTest", "Create thread fail.\n");
}

void SocketsInitCall(JNIEnv *R_Env, jobject thiz, jstring jip)
{
	const char *ip;
	int  EventNumber;
	ip= (*R_Env)->GetStringUTFChars(R_Env, jip, NULL); // Java String to C Style string
	LOG(I, "SocketsInitCall", "ReleaseString remove");
	LOG(I, "SocketsInitCall", "SocketsInitCall and ip : %s", ip);
	if(SocketsInit(ip) == 0)
	{
		LOG(I, "SocketsInitCall", "SocketsInit fail");
		return;
	}
	LOG(I, "SocketsInitCall", "SocketsInit(ip)\n");
	SocketThreadFlag = 1;

	EventNumber = MatchingEventDevice();
	sprintf(EventFileName, "/dev/input/event%d", EventNumber);
	LOG(I, "SocketsInitCall", "%s", EventFileName);
	EventDeviceAuthorityChange(env, EventFileName);

	Socket_thr_id = pthread_create(&Socket_thread, NULL, Socket_function, (void *)NULL);
	if(Playing_thr_id < 0)
		LOG(I, "SocketsInitCall", "Create thread fail.\n");
}

void SocketClose(JNIEnv *R_Env, jobject thiz)
{
	SocketThreadFlag = 0;
	SocketsDestroy();
}

jint Display(JNIEnv *env, jobject thiz, jint index)
{
	if(index == 1)
		return (int)(resource[index] * 100);
	return (int)(resource[index]);
}

static JNINativeMethod methods[] = {
		{"PlayInputTest", "(IIII)V", (void*)PlayInputTest},
		{"RecordStart", "(II)V", (void*)RecordStart},
		{"RecordStop", "()V", (void*)RecordStop},
		{"nativeSocketInit", "(Ljava/lang/String;)V", (void*)SocketsInitCall },
		{"nativeSocketClose", "()V", (void*)SocketClose},
		{"nativeDisplay", "(I)I", (void*)Display},
};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	jint result = -1;

	if((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		LOG(I, "NATIVE", "GetEnv failed.\n");
		goto error;
	}

	cls = (*env)->FindClass(env, PAKAGE_CLASS_NAME);
	if(cls == NULL)
	{
		LOG(I, "NATIVE", "Native registration unable to find class(InputDevice)");
		goto error;
	}

	if((*env)->RegisterNatives(env, cls, methods, sizeof(methods)/sizeof(methods[0])) < 0){
		LOG(I, "NATIVE", "Native function registration fail.");
		goto error;
	}

	LOG(I, "NATIVE", "RegisterNatives success....\n");

	result = JNI_VERSION_1_4;
	glpVM = vm;
error:
	return result;
}
