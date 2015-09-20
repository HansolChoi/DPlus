#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include "InputDevice/input.h"

#define PAKAGE_CLASS_NAME	"NativeService/InputDevice"
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
	ReadingLoop(EventFileName, &RecordingThreadFlag, ScreenWidth, ScreenHeight);
}

void *Playing_function(void *data)
{
	PlayingLoop(EventFileName, &RecordingThreadFlag, ScreenWidth, ScreenHeight);
}

void RecordStart(JNIEnv *R_Env, jobject thiz, jint Width, jint Height)
{
	RecordingThreadFlag = 1;
	int  EventNumber;

	EventNumber = MatchingEventDevice();
	sprintf(EventFileName, "/dev/input/event%d", EventNumber);
	LOG(ANDROID_LOG_INFO, "THREAD", "%s", EventFileName);

	ScreenWidth = Width; ScreenHeight = Height;
	EvenvDeviceAuthorityChange(env, EventFileName);

	Recording_thr_id = pthread_create(&Recording_thread, NULL, Recording_function, (void *)NULL);
	if(Recording_thr_id < 0)
		LOG(ANDROID_LOG_INFO, "THREAD", "Create thread fail.\n");
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
	LOG(ANDROID_LOG_INFO, "THREAD", "%s", EventFileName);

	ScreenWidth = Width; ScreenHeight = Height;
	EvenvDeviceAuthorityChange(env, EventFileName);

	Playing_thr_id = pthread_create(&Playing_thread, NULL, Playing_function, (void *)NULL);
	if(Playing_thr_id < 0)
		LOG(ANDROID_LOG_INFO, "THREAD", "Create thread fail.\n");
}

static JNINativeMethod methods[] = {
		{"PlayInputTest", "(II)V", (void*)PlayInputTest},
		{"RecordStart", "(II)V", (void*)RecordStart},
		{"RecordStop", "()V", (void*)RecordStop},
};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	jint result = -1;

	if((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "GetEnv failed.\n");
		goto error;
	}

	cls = (*env)->FindClass(env, PAKAGE_CLASS_NAME);
	if(cls == NULL)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "Native registration unable to find class(InputDevice)");
		goto error;
	}

	if((*env)->RegisterNatives(env, cls, methods, sizeof(methods)/sizeof(methods[0])) < 0){
		LOG(ANDROID_LOG_INFO, "NATIVE", "Native function registration fail.");
		goto error;
	}

	LOG(ANDROID_LOG_INFO, "NATIVE", "RegisterNatives success....\n");

	result = JNI_VERSION_1_4;
	glpVM = vm;
error:
	return result;
}
