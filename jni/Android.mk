LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE	:= nativeLib
LOCAL_SRC_FILES := Socket/socket.c init.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)