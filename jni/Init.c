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
pthread_t Recording_thread;
pthread_t Playing_thread;
int RecordingThreadFlag;
int PlayingThreadFlag;

char EventFileName[30];
int ScreenWidth, ScreenHeight;

void *Recording_function(void *data)
{
	Record(EventFileName, &RecordingThreadFlag, ScreenWidth, ScreenHeight);
}

void *Playing_function(void *data)
{
	Replay(EventFileName, &RecordingThreadFlag, ScreenWidth, ScreenHeight);
}

void RecordStart(JNIEnv *R_Env, jobject thiz, jint Width, jint Height)
{
	RecordingThreadFlag = 1;
	int  EventNumber;

	ScreenWidth = Width; ScreenHeight = Height;
	EventNumber = MatchingEventDevice();

	sprintf(EventFileName, "/dev/input/event%d", EventNumber);
	LOG(I, "RecordStart", "%s", EventFileName);

	EventDeviceAuthorityChange(env, EventFileName);

	Recording_thr_id = pthread_create(&Recording_thread, NULL, Recording_function, (void *)NULL);
	if(Recording_thr_id < 0)
		LOG(I, "RecordStart", "Create thread fail.\n");
}

void RecordStop()
{
	RecordingThreadFlag = 0;
}

void PlayInputTest(JNIEnv *R_Env, jobject thiz, jint Width, jint Height)
{
	PlayingThreadFlag = 1;
	int  EventNumber;

	EventNumber = MatchingEventDevice();
	sprintf(EventFileName, "/dev/input/event%d", EventNumber);
	LOG(I, "PlayInputTest", "%s", EventFileName);

	ScreenWidth = Width; ScreenHeight = Height;
	EventDeviceAuthorityChange(env, EventFileName);

	Playing_thr_id = pthread_create(&Playing_thread, NULL, Playing_function, (void *)NULL);
	if(Playing_thr_id < 0)
		LOG(I, "PlayInputTest", "Create thread fail.\n");
}


// This function transfer data to Android MainActivity.
// Function parameter mean that index of resource array.
// Display(2) return resource[2] *100. Multiple 100 make bottom action.
// (3.12) *100 -> 312 -> 312/100 = 3.12
jint Display(JNIEnv *env, jobject thiz, jint index)
{
	if(index == 1)
		return (int)(resource[index] * 100);
	return (int)(resource[index]);
}

void SocketsInitCall(JNIEnv *env, jobject thiz, jstring jip)
{
	const char *ip;
	ip= (*env)->GetStringUTFChars(env, jip, NULL); // Java String to C Style string

	SocketsInit(ip);
}

static JNINativeMethod methods[] = {
		{"PlayInputTest", "(II)V", (void*)PlayInputTest},
		{"RecordStart", "(II)V", (void*)RecordStart},
		{"RecordStop", "()V", (void*)RecordStop},
		{"nativeSocketInit", "(Ljava/lang/String;)V", (void*)SocketsInitCall },
		{"nativeSocketClose", "()V", (void*)SocketsDestroy},
		{"nativeResourceTransfer", "()V", (void*)SocketsTransfer},
		{"nativeDisplay", "(I)I", (void*)Display},
		{"nativewaitpid", "()V", (void*)_waitpid},
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
