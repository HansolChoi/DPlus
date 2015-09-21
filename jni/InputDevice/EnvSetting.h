#ifndef _INPUT_H_
#define _INPUT_H_

int MatchingEventDevice();
void EventDeviceAuthorityChange(JNIEnv* env, const char* file);

void JavaShellEnv(JNIEnv* env);
void JavaShell(JNIEnv* env, const char* param);

#endif
