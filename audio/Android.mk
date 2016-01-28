# Copyright 2011 The Android Open Source Project

#AUDIO_POLICY_TEST := true
#ENABLE_AUDIO_DUMP := true

LOCAL_PATH := $(call my-dir)

common_cflags := -D_POSIX_SOURCE
ifeq ($(strip $(AUDIO_FEATURE_ENABLED_FM)),true)
    common_cflags += -DQCOM_FM_ENABLED
endif

ifneq ($(strip $(QCOM_PROXY_DEVICE_ENABLED)),false)
    common_cflags += -DQCOM_PROXY_DEVICE_ENABLED
endif

ifneq ($(strip $(QCOM_OUTPUT_FLAGS_ENABLED)),false)
    common_cflags += -DQCOM_OUTPUT_FLAGS_ENABLED
endif

ifneq ($(strip $(QCOM_TUNNEL_LPA_ENABLED)),false)
    common_cflags += -DQCOM_TUNNEL_LPA_ENABLED
endif

ifneq ($(strip $(BOARD_QCOM_VOIP_ENABLED)),false)
  common_cflags += -DQCOM_VOIP_ENABLED
endif

ifeq ($(strip $(BOARD_USES_SRS_TRUEMEDIA)),true)
  common_cflags += -DSRS_PROCESSING
endif

include $(CLEAR_VARS)

LOCAL_CFLAGS += $(common_cflags)

LOCAL_SRC_FILES := \
    audio_hw_hal.cpp \
    HardwarePinSwitching.c

LOCAL_SRC_FILES += \
    AudioHardware.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils       \
    libutils        \
    libmedia

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

LOCAL_STATIC_LIBRARIES := \
    libmedia_helper \
    libaudiohw_legacy

LOCAL_MODULE := audio.primary.msm7x27a
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -fno-short-enums

LOCAL_C_INCLUDES += hardware/libhardware/include
LOCAL_C_INCLUDES += hardware/libhardware_legacy/include
LOCAL_C_INCLUDES += frameworks/base/include
LOCAL_C_INCLUDES += system/core/include

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

include $(BUILD_SHARED_LIBRARY)

ifeq ($(USE_LEGACY_AUDIO_POLICY), 1)
# The audio policy is implemented on top of legacy policy code
include $(CLEAR_VARS)

LOCAL_CFLAGS += $(common_cflags)

LOCAL_SRC_FILES := \
    AudioPolicyManager.cpp \
    audio_policy_hal.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libmedia

LOCAL_STATIC_LIBRARIES := \
    libaudiohw_legacy \
    libmedia_helper \
    libaudiopolicy_legacy

LOCAL_MODULE := audio_policy.msm7x27a
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := hardware/libhardware_legacy/audio

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

include $(BUILD_SHARED_LIBRARY)

endif
