LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE	:= NativeService
LOCAL_SRC_FILES := init.c InputDevice/EnvSetting.c InputDevice/record.c InputDevice/replay.c Resource/logcat.c Resource/resource.c Resource/socket.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)