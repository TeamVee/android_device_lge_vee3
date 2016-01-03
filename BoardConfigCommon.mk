# Copyright (C) 2015 The CyanogenMod Project
# Copyright (C) 2015 The TeamVee Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# 	http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# inherit from the common proprietary version
-include vendor/lge/vee-common/BoardConfigVendor.mk

BOARD_VENDOR := lge

# Compiler flags
TARGET_GLOBAL_CFLAGS += -mfloat-abi=softfp -mfpu=neon-vfpv4
TARGET_GLOBAL_CPPFLAGS += -mfloat-abi=softfp -mfpu=neon-vfpv4
COMMON_GLOBAL_CFLAGS += -DUSE_MDP3
COMMON_GLOBAL_CFLAGS += -DLPA_DEFAULT_BUFFER_SIZE=480
TARGET_RELEASE_CPPFLAGS += -DNEEDS_VECTORIMPL_SYMBOLS
COMMON_GLOBAL_CFLAGS += -DQCOM_DIRECTTRACK
COMMON_GLOBAL_CFLAGS += -DQCOM_LEGACY_UIDS
COMMON_GLOBAL_CFLAGS += -DQCOM_BSP_LEGACY
COMMON_GLOBAL_CFLAGS += -DBOARD_CANT_REALLOCATE_OMX_BUFFERS
COMMON_GLOBAL_CFLAGS += -DNO_SECURE_DISCARD

# Targets
TARGET_AVOID_DRAW_TEXTURE_EXTENSION := true
TARGET_USES_16BPPSURFACE_FOR_OPAQUE := true

TARGET_NO_BOOTLOADER := true
TARGET_BOOTLOADER_BOARD_NAME := vee
TARGET_BOARD_PLATFORM := msm7x27a
TARGET_BOARD_PLATFORM_GPU := qcom-adreno200
TARGET_GRALLOC_USES_ASHMEM := true

TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_VARIANT := cortex-a9
ARCH_ARM_HAVE_TLS_REGISTER := true
TARGET_CORTEX_CACHE_LINE_32 := true
ARCH_ARM_HAVE_32_BYTE_CACHE_LINES := true

# Kernel
TARGET_KERNEL_SOURCE := kernel/lge/msm7x27a-common
BOARD_KERNEL_BASE := 0x00200000
BOARD_KERNEL_PAGESIZE := 4096

# Qualcomm hardware
BOARD_USES_QCOM_HARDWARE := true
TARGET_USE_QCOM_BIONIC_OPTIMIZATION := true

# GPS
BOARD_USES_QCOM_LIBRPC := true
BOARD_USES_QCOM_GPS := true
BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION := 50000
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := $(TARGET_BOARD_PLATFORM)

# Audio
BOARD_USES_LEGACY_ALSA_AUDIO := true

# BIONIC: use legacy mmap
BOARD_USES_LEGACY_MMAP := true

# Display
BOARD_ADRENO_DECIDE_TEXTURE_TARGET := true
USE_OPENGL_RENDERER := true
TARGET_USES_ION := true
TARGET_USES_QCOM_BSP := true
TARGET_DISPLAY_USE_RETIRE_FENCE := true
TARGET_NO_INITLOGO := true

# Screenshot
BOARD_USE_MHEAP_SCREENSHOT := true

# Hardware
BOARD_HARDWARE_CLASS := device/lge/vee-common/cmhw

# BT
BOARD_HAVE_BLUETOOTH := true

# Light
TARGET_PROVIDES_LIBLIGHT := true

# Camera
USE_DEVICE_SPECIFIC_CAMERA := true

# Enable Minikin text layout engine (will be the default soon)
USE_MINIKIN := true

# Use dlmalloc instead of jemalloc for mallocs on low-ram targets
MALLOC_IMPL := dlmalloc

# Enable dex-preoptimization to speed up first boot sequence
ifeq ($(HOST_OS),linux)
	WITH_DEXPREOPT := true
endif
WITH_DEXPREOPT_PIC := true
DONT_DEXPREOPT_PREBUILTS := true

# Include an expanded selection of fonts
EXTENDED_FONT_FOOTPRINT := true

# Block based Over-The-Air
BLOCK_BASED_OTA := false

# SEPolicy
include device/qcom/sepolicy/sepolicy.mk

# Bootanimation
TARGET_BOOTANIMATION_PRELOAD := true
TARGET_BOOTANIMATION_TEXTURE_CACHE := true

# Wlan
BOARD_HAS_QCOM_WLAN := true
BOARD_HAS_QCOM_WLAN_SDK := true
BOARD_WLAN_DEVICE := qcwcn
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
WIFI_EXT_MODULE_PATH := "/system/lib/modules/librasdioif.ko"
WIFI_EXT_MODULE_NAME := "librasdioif"
WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wlan.ko"
WIFI_DRIVER_MODULE_NAME := "wlan"
WIFI_DRIVER_FW_PATH_STA := "sta"
WIFI_DRIVER_FW_PATH_AP := "ap"
WIFI_DRIVER_FW_PATH_P2P := "p2p"
WIFI_DRIVER_FW_PATH_PARAM := "/data/misc/wifi/fwpath"

# Recovery
RECOVERY_GRAPHICS_USE_LINELENGTH := true
RECOVERY_FSTAB_VERSION := 2
TARGET_RECOVERY_PIXEL_FORMAT := "RGB_565"
TARGET_RECOVERY_QCOM_RTC_FIX := true
BOARD_USE_CUSTOM_RECOVERY_FONT := "<font_7x16.h>"
BOARD_USES_MMCUTILS := true
BOARD_HAS_NO_MISC_PARTITION := true
BOARD_HAS_NO_SELECT_BUTTON := true

# TWRP
RECOVERY_VARIANT := omni
TW_INTERNAL_STORAGE_PATH := "/data/media"
TW_INTERNAL_STORAGE_MOUNT_POINT := "data"
TW_EXTERNAL_STORAGE_PATH := "/external sd"
TW_EXTERNAL_STORAGE_MOUNT_POINT := "external_sd"
TW_DEFAULT_EXTERNAL_STORAGE := true
TW_FLASH_FROM_STORAGE := true
TW_NO_CPU_TEMP := true
TW_NO_SCREEN_TIMEOUT := true
TW_BRIGHTNESS_PATH := /sys/class/leds/lcd-backlight/brightness
TW_MAX_BRIGHTNESS := 225
