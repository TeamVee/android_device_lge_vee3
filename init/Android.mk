LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := system/core/init
LOCAL_SRC_FILES := init_vee3.cpp
LOCAL_MODULE := libinit_vee3
include $(BUILD_STATIC_LIBRARY)
