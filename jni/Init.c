#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

//#include "Socket/socket.h"

#define PAKAGE_CLASS_NAME	"Socket/Socket"

JavaVM *glpVM = NULL;
jclass jObject;
jmethodID funccb;
jclass cls;

// [2stage] This function table make to register in JIN environment.
static JNINativeMethod methods[] = {
		{"CreateSocket", "(I)I", (void*)CreateSocket},
		{"SocketSend", "(Ljava/lang/String;II)I", (void*)SocketSend},
		{"SocketReceive", "(I)Ljava/lang/String;", (void*)SocketReceive},
		{"SocketStop", "(I)V", (void*)SocketStop},
};

// [1stage] If Android class NDK_Onload library call, JIN_OnLoad function is executed.
// JNI_OnLoad register function table.
jint JNI_OnLoad(JavaVM* vm, void* reserved){
	jint result = -1;
	JNIEnv* env = NULL;

	__android_log_print(ANDROID_LOG_INFO, "NATIVE", "Native registration process.");

	if((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK){
		__android_log_print(ANDROID_LOG_INFO, "NATIVE", "GetEnv failed.\n");
		goto error;
	}

	cls = (*env)->FindClass(env, PAKAGE_CLASS_NAME);
	if(cls == NULL){
		__android_log_print(ANDROID_LOG_INFO, "NATIVE", "Native registration unable to find class(AVMJni)");
		goto error;
	}

	jObject = (jclass)(*env)->NewGlobalRef(env, cls);

	// Native connect.
	if((*env)->RegisterNatives(env, cls, methods, sizeof(methods)/sizeof(methods[0]))<0){
		__android_log_print(ANDROID_LOG_INFO, "NATIVE", "RegisterNatives failed !!!\n");
		goto error;
	}
	__android_log_print(ANDROID_LOG_INFO, "NATIVE", "RegisterNatives success....\n");

	result = JNI_VERSION_1_4;
	glpVM = vm;
error:
	return result;
}
