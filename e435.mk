#
# Copyright (C) 2014 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# The gps config appropriate for this device
$(call inherit-product, device/common/gps/gps_us_supl.mk)

$(call inherit-product-if-exists, vendor/lge/e435/e435-vendor.mk)

$(call inherit-product-if-exists, vendor/lge/msm7x27a-common/msm7x27a-common-vendor.mk)

DEVICE_PACKAGE_OVERLAYS += device/lge/e435/overlay

$(call inherit-product, frameworks/native/build/phone-hdpi-512-dalvik-heap.mk)

PRODUCT_AAPT_CONFIG := normal mdpi ldpi
PRODUCT_AAPT_PREF_CONFIG := ldpi

# Rootdir
PRODUCT_PACKAGES += \
    fstab.vee3 \
    init.vee3.rc \
    init.vee3.usb.rc \
    ueventd.vee3.rc \
    init.qcom.usb.sh \
    init.qcom.class_main.sh \
    init.qcom.sh

# Configs
PRODUCT_PACKAGES += \
    vee3_keypad.kcm \
    7k_handset.kl \
    7x27a_kp.kl \
    touch_mcs8000.kl \
    vee3_keypad.kl \
    AudioFilter.csv \
    media_codecs.xml \
    media_profiles.xml \
    wpa_supplicant.conf \
    vold.fstab \
    hostapd.accept \
    hostapd_default.conf \
    hostapd.deny

# Wlan
PRODUCT_PACKAGES += \
    WCN1314_qcom_wlan_nv.bin \
    WCN1314_qcom_fw.bin \
    WCN1314_cfg.dat \
    WCN1314_qcom_cfg.ini

# Permission files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml

# Audio
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.primary.msm7x27a \
    audio_policy.conf \
    audio_policy.msm7x27a \
    audio.usb.default \
    libaudioparameter \
    libaudio-resampler \
    libtinyalsa \
    tinycap \
    tinymix \
    tinyplay

# Video
PRODUCT_PACKAGES += \
    libI420colorconvert \
    libstagefrighthw \
    libmm-omxcore \
    libOmxCore

# GPS
PRODUCT_PACKAGES += \
    gps.msm7x27a \
    gps.default \
    libgps.utils \
    libloc_adapter \
    libloc_eng \
    libloc_api-rpc-qc

# Display
PRODUCT_PACKAGES += \
    copybit.msm7x27a \
    gralloc.msm7x27a \
    hwcomposer.msm7x27a \
    memtrack.msm7x27a \
    libc2dcolorconvert \
    liboverlay \
    libmemalloc \
    libtilerenderer \
    libgenlock \
    libqdutils \
    libqdMetaData

# Light
PRODUCT_PACKAGES += \
    lights.msm7x27a

# FM Radio
PRODUCT_PACKAGES += \
    FM2 \
    FMRecord \
    libqcomfm_jni \
    qcom.fmradio

# Power Hal
PRODUCT_PACKAGES += \
    power.msm7x27a

# Camera
PRODUCT_PACKAGES += \
    camera.msm7x27a

# Omx
PRODUCT_PACKAGES += \
    libmm-omxcore \
    libOmxCore \
    libstagefrighthw \
    libdashplayer

# off-mode charging
PRODUCT_PACKAGES += \
    charger \
    charger_res_images

# Filesystem management tools
PRODUCT_PACKAGES += \
    make_ext4fs \
    e2fsck \
    setup_fs

# Misc
PRODUCT_PACKAGES += \
    com.android.future.usb.accessory

# Build.prop Properties
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp

PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=/system/lib/libqc-opt.so

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=120

# Voice processing
PRODUCT_PACKAGES += libqcomvoiceprocessing

PRODUCT_PACKAGES += Torch

# For userdebug builds
ADDITIONAL_DEFAULT_PROPERTIES += \
	ro.secure=0 \
	ro.adb.secure=0

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_UTC_DATE=0
PRODUCT_NAME := full_e435
PRODUCT_DEVICE := e435
