LOCAL_PATH := $(call my-dir)

# Remove CMUpdater
LOCAL_OVERRIDES_PACKAGES := CMUpdater

# Template
#
#include $(CLEAR_VARS)
#LOCAL_MODULE       := Template
#LOCAL_SRC_FILES    := app/$(LOCAL_MODULE).apk
#LOCAL_OVERRIDES_PACKAGES := Template
#LOCAL_MODULE_CLASS := APPS
#LOCAL_MODULE_TAGS  := optional
#LOCAL_MODULE_PATH  := system/app/$(LOCAL_MODULE)/$(LOCAL_MODULE).apk
#include $(BUILD_PREBUILT)
