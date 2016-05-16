# Copyright (C) 2015 The CyanogenMod Project
# Copyright (C) 2016 The TeamVee Project
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
#

# Delete the '#' if you are building to L1II or use 'export TARGET_KERNEL_V1_BUILD_DEVICE=true' before build
#TARGET_KERNEL_V1_BUILD_DEVICE := true

# HardCode Device Local Path
DEVICE_LOCAL_PATH:= device/lge/vee3

PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := ldpi

$(call inherit-product, frameworks/native/build/phone-hdpi-512-dalvik-heap.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)
$(call inherit-product, device/common/gps/gps_us_supl.mk)

# inherit from the proprietary version
$(call inherit-product, vendor/lge/msm7x27a-common/msm7x27a-common-vendor.mk)
ifeq ($(TARGET_KERNEL_V1_BUILD_DEVICE),true)
$(call inherit-product, vendor/lge/v1/v1-vendor.mk)
else
$(call inherit-product, vendor/lge/vee3/vee3-vendor.mk)
endif

DEVICE_PACKAGE_OVERLAYS += $(DEVICE_LOCAL_PATH)/overlay

# Rootdir files
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/root/fstab.vee3:root/fstab.vee3
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/root/init.vee3.rc:root/init.vee3.rc
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/root/init.vee3ds.sh:root/init.vee3ds.sh
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/root/init.vee3.usb.rc:root/init.vee3.usb.rc
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/root/ueventd.vee3.rc:root/ueventd.vee3.rc

# Config Files
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/usr/keylayout/7k_handset.kl:system/usr/keylayout/7k_handset.kl
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/usr/keylayout/7x27a_kp.kl:system/usr/keylayout/7x27a_kp.kl
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/usr/keylayout/mms100s_ts.kl:system/usr/keylayout/mms100s_ts.kl
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/usr/keylayout/touch_mcs8000.kl:system/usr/keylayout/touch_mcs8000.kl
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/usr/keylayout/v1_keypad.kl:system/usr/keylayout/v1_keypad.kl
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/usr/keylayout/vee3_keypad.kl:system/usr/keylayout/vee3_keypad.kl
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/firmware/wlan/volans/WCN1314_cfg.dat:system/etc/firmware/wlan/volans/WCN1314_cfg.dat
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/firmware/wlan/volans/WCN1314_qcom_cfg.ini:system/etc/firmware/wlan/volans/WCN1314_qcom_cfg.ini
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/firmware/wlan/volans/WCN1314_qcom_fw.bin:system/etc/firmware/wlan/volans/WCN1314_qcom_fw.bin
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/firmware/wlan/volans/WCN1314_qcom_wlan_nv.bin:system/etc/firmware/wlan/volans/WCN1314_qcom_wlan_nv.bin
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/hostapd/hostapd.accept:system/etc/hostapd/hostapd.accept
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/hostapd/hostapd.deny:system/etc/hostapd/hostapd.deny
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/hostapd/hostapd_default.conf:system/etc/hostapd/hostapd_default.conf
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/audio_policy.conf:system/etc/audio_policy.conf
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/media_codecs.xml:system/etc/media_codecs.xml
PRODUCT_COPY_FILES += $(DEVICE_LOCAL_PATH)/rootdir/system/etc/media_profiles.xml:system/etc/media_profiles.xml

# Special Google Media Codecs
PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml
PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml
PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:system/etc/media_codecs_google_video_le.xml

# Permission files
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.xml:system/etc/permissions/android.hardware.touchscreen.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml

# Display HAL
PRODUCT_PACKAGES += copybit.msm7x27a
PRODUCT_PACKAGES += gralloc.msm7x27a
PRODUCT_PACKAGES += memtrack.msm7x27a
PRODUCT_PACKAGES += libqdMetaData

# Media HAL
PRODUCT_PACKAGES += libstagefrighthw
PRODUCT_PACKAGES += libmm-omxcore
PRODUCT_PACKAGES += libOmxCore
PRODUCT_PACKAGES += libdashplayer

# Off-mode Charging
PRODUCT_PACKAGES += charger
PRODUCT_PACKAGES += charger_res_images

# GPS HAL
PRODUCT_PACKAGES += gps.msm7x27a

# Power Hal
PRODUCT_PACKAGES += power.msm7x27a

# BT HAL
PRODUCT_PACKAGES += libbt-vendor

# EXT4
PRODUCT_PACKAGES += make_ext4fs
PRODUCT_PACKAGES += e2fsck
PRODUCT_PACKAGES += setup_fs

# Usb
PRODUCT_PACKAGES += com.android.future.usb.accessory

# Audio HAL
PRODUCT_PACKAGES += audio.a2dp.default
PRODUCT_PACKAGES += audio.usb.default
PRODUCT_PACKAGES += audio.r_submix.default
PRODUCT_PACKAGES += audio.primary.msm7x27a
PRODUCT_PACKAGES += audio_policy.msm7x27a
PRODUCT_PACKAGES += libaudio-resampler
PRODUCT_PACKAGES += libaudioparameter
PRODUCT_PACKAGES += libaudioutils

# Light HAL
PRODUCT_PACKAGES += lights.msm7x27a

# Camera Hal
PRODUCT_PACKAGES += camera.msm7x27a

PRODUCT_DEX_PREOPT_DEFAULT_FLAGS := --compiler-filter=interpret-only
$(call add-product-dex-preopt-module-config,services,--compiler-filter=space)

# WiFi
PRODUCT_PACKAGES += libwpa_client
PRODUCT_PACKAGES += hostapd
PRODUCT_PACKAGES += dhcpcd.conf
PRODUCT_PACKAGES += wpa_supplicant
PRODUCT_PACKAGES += wpa_supplicant.conf
PRODUCT_PACKAGES += libQWiFiSoftApCfg
PRODUCT_PACKAGES += libnetcmdiface

# Stlport
PRODUCT_PACKAGES +=  libstlport

include $(DEVICE_LOCAL_PATH)/system_prop.mk
