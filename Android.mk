LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := NdkCamera.cpp

LOCAL_C_INCLUDES := frameworks/av/include \
					frameworks/native/include \
					frameworks/base/include/ui

LOCAL_SHARED_LIBRARIES:= libui libgui libutils libcutils \
						 libbinder libcamera_client liblog

LOCAL_CFLAGS := -Wall -Werror -Wno-unused-parameter

LOCAL_MULTILIB := 64

LOCAL_MODULE := ndkcamera

include $(BUILD_EXECUTABLE)