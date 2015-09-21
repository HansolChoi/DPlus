#ifndef _INPUT_H_
#define _INPUT_H_

#define EVENT_BUF_NUM	64

int MatchingEventDevice();
void EvenvDeviceAuthorityChange(JNIEnv* env, const char* file);
void GetSceenResolution(JNIEnv* env, int* width, int* height);

void JavaShellEnv(JNIEnv* env);
void JavaShell(JNIEnv* env, const char* param);
void ReadingLoop(const char* event, int* flag, int ScreenWidth, int ScreenHeight);
void PlayingLoop(const char* file, int* flag, int ScreenWidth, int ScreenHeight );

#endif
