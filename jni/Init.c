#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "Socket/socket.h"

#define PAKAGE_CLASS_NAME	"tjssm/ServerListen/NativeCaller"
#define LOG	__android_log_print

JavaVM *glpVM = NULL;
jclass jObject;
jmethodID funccb;
jclass cls;

void ServerCommandListen(JNIEnv* env, jobject thiz, jstring _ip, jint port)
{
	const char *ip;
	ip= (*env)->GetStringUTFChars(env, _ip, NULL); // Java String to C Style string

	LOG(ANDROID_LOG_INFO, "NATIVE", "ServerCommandListen is called.");

	CreateSocket(ip, port);
	SocketSend("hansol's first message", 23, port);
	SocketStop(port);
}

void LogcatExtract(JNIEnv* env, jobject thiz)
{
	LOG(ANDROID_LOG_INFO, "NATIVE", "LogcatExtract is called.");
}

void CommandLineTool(JNIEnv* env, jobject thiz)
{
	LOG(ANDROID_LOG_INFO, "NATIVE", "CommandLineTool is called.");
}

void ResourceExtract(JNIEnv* env, jobject thiz)
{
	LOG(ANDROID_LOG_INFO, "NATIVE", "ResourceExtract is called.");
}

void NativeServiceStop(JNIEnv* env, jobject thiz)
{
	LOG(ANDROID_LOG_INFO, "NATIVE", "NativeServiceStop is called.");
}

static JNINativeMethod methods[] = {
		{ "ServerCommandListen", "(Ljava/lang/String;I)V", (void*)ServerCommandListen },
		{ "LogcatExcute", "()V", (void*)LogcatExtract },
		{ "CommandLineTool", "()V", (void*)CommandLineTool },
		{ "ResourceExtract", "()V", (void*)ResourceExtract },
		{ "NativeServiceStop", "()V", (void*)NativeServiceStop },
};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	jint result = -1;
	JNIEnv* env = NULL;

	if((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "GetEnv failed.\n");
		goto error;
	}

	cls = (*env)->FindClass(env, PAKAGE_CLASS_NAME);
	if(cls == NULL)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "Native registration unable to find class(AVMJni)");
		goto error;
	}

	// Native connect.
	if((*env)->RegisterNatives(env, cls, methods, sizeof(methods)/sizeof(methods[0]))<0)
	{
		LOG(ANDROID_LOG_INFO, "NATIVE", "RegisterNatives failed !!!\n");
		goto error;
	}

	LOG(ANDROID_LOG_INFO, "NATIVE", "RegisterNatives success....\n");

	result = JNI_VERSION_1_4;
	glpVM = vm;
error:
	return result;
}
