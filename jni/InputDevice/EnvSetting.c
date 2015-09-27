#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <jni.h>

#include "EnvSetting.h"
#include "record.h"

#define LOG	__android_log_print

jclass sh_cls;
jmethodID func;
jobject sh_obj;

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
void EventDeviceAuthorityChange(JNIEnv* env, const char* file)
{
	char chmod[100] = "chmod 777 ";

	JavaShellEnv(env);

	JavaShell(env, "su"); // 루트권한 획득
	strcat(chmod, file);
	JavaShell(env, chmod); // 권한 변경
	LOG(ANDROID_LOG_INFO, "EventDeviceAuthorityChange", "권한변경했어.");
}

/*
 * 쉘 환경설정
 */
void JavaShellEnv(JNIEnv* env)
{
	LOG(ANDROID_LOG_INFO, "JavaShellEnv", "클래스 찾기");
	// 클래스  찾기
	sh_cls = (*env)->FindClass(env, "Shell/ShellReceiver");
	if(sh_cls == NULL){
		LOG(ANDROID_LOG_INFO, "JavaShellEnv", "클래스 검색 실패");
		return;
	}

	LOG(ANDROID_LOG_INFO, "JavaShellEnv", "생성자 찾기");
	// 생성자 찾기
	func = (*env)->GetMethodID(env, sh_cls, "<init>", "()V");
	if(func == NULL){
		LOG(ANDROID_LOG_INFO, "JavaShellEnv", "생성자를 못찾음");
		return;
	}

	LOG(ANDROID_LOG_INFO, "JavaShellEnv", "객체 생성");
	// 객체 생성
	sh_obj = (*env)->NewObject(env, sh_cls, func);
	if(sh_obj == NULL){
		LOG(ANDROID_LOG_INFO, "JavaShellEnv", "객체 생성 실패");
		return;
	}

	LOG(ANDROID_LOG_INFO, "JavaShellEnv", "메소드 호출");
	// 객체의 메소드 호출
	func = (*env)->GetMethodID(env, sh_cls, "ShellCmd", "(Ljava/lang/String;)V");
	if(func == NULL){
		LOG(ANDROID_LOG_INFO, "JavaShellEnv", "객체 메소드 찾기 실패");
		return;
	}
	LOG(ANDROID_LOG_INFO, "JavaShellEnv", "자바쉘 등록완료");
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
