LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE	:= InputService
LOCAL_SRC_FILES := init.c InputDevice/EnvSetting.c InputDevice/record.c InputDevice/replay.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)