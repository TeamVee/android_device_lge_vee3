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

# For userdebug builds
ifeq ($(TARGET_BUILD_VARIANT),userdebug)
ADDITIONAL_DEFAULT_PROPERTIES += ro.secure="false"
ADDITIONAL_DEFAULT_PROPERTIES += ro.adb.secure="false"
endif
# Lcd Density
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density="120"
# Enforce Phone Mode view
PRODUCT_PROPERTY_OVERRIDES += ro.screen.layout="normal"
# QCOM
PRODUCT_PROPERTY_OVERRIDES += com.qc.hardware="true"
# Vendor Lib
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.extension_library="/system/lib/libqc-opt.so"
# OpenGL
PRODUCT_PROPERTY_OVERRIDES += ro.opengles.version="131072"
# WiFi
PRODUCT_PROPERTY_OVERRIDES += wifi.interface="wlan0"
# Display Composition
PRODUCT_PROPERTY_OVERRIDES += debug.composition.type="dyn"
PRODUCT_PROPERTY_OVERRIDES += persist.hwc.mdpcomp.enable="false"
# Use Hardware Rendering
PRODUCT_PROPERTY_OVERRIDES += debug.sf.hw="1"
# Media StageFright
PRODUCT_PROPERTY_OVERRIDES += media.stagefright.enable-player="true"
PRODUCT_PROPERTY_OVERRIDES += media.stagefright.enable-meta="false"
PRODUCT_PROPERTY_OVERRIDES += media.stagefright.enable-scan="true"
PRODUCT_PROPERTY_OVERRIDES += media.stagefright.enable-http="true"
PRODUCT_PROPERTY_OVERRIDES += media.stagefright.enable-fma2dp="true"
PRODUCT_PROPERTY_OVERRIDES += media.stagefright.enable-aac="true"
PRODUCT_PROPERTY_OVERRIDES += media.stagefright.enable-qcp="true"
# RIL
PRODUCT_PROPERTY_OVERRIDES += ril.subscription.types="NV,RUIM"
PRODUCT_PROPERTY_OVERRIDES += ro.telephony.ril_class="JSRQualcommRIL"
PRODUCT_PROPERTY_OVERRIDES += ro.telephony.ril.config="qcomdsds,skippinpukcount,simactivation"
PRODUCT_PROPERTY_OVERRIDES += ro.telephony.call_ring.multiple="false"
PRODUCT_PROPERTY_OVERRIDES += ro.telephony.call_ring="false"
# GSM Preferred
PRODUCT_PROPERTY_OVERRIDES += ro.telephony.default_network="3"
# RILD
PRODUCT_PROPERTY_OVERRIDES += rild.libpath="/system/lib/libril-qc-qmi-1.so"
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_plmn=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_long_ons_0=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_long_ons_1=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_long_ons_2=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_long_ons_3=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_short_ons_0=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_short_ons_1=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_short_ons_2=""
PRODUCT_PROPERTY_OVERRIDES += persist.rild.nitz_short_ons_3=""
# Data
PRODUCT_PROPERTY_OVERRIDES += ro.use_data_netmgrd="true"
PRODUCT_PROPERTY_OVERRIDES += persist.data.ds_fmc_app.mode="0"
# SIM
PRODUCT_PROPERTY_OVERRIDES += persist.service.factory.enable="0"
PRODUCT_PROPERTY_OVERRIDES += persist.radio.apm_sim_not_pwdn="1"
# Debug
PRODUCT_PROPERTY_OVERRIDES += debug.gralloc.map_fb_memory="1"
PRODUCT_PROPERTY_OVERRIDES += debug.hwc.fakevsync="1"
# Audio
PRODUCT_PROPERTY_OVERRIDES += audio.offload.disable="1"
# ART
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.dex2oat-filter="balanced"
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.dex2oat-flags="--no-watch-dog"
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.image-dex2oat-filter="speed"
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.dex2oat-swap="false"
# For applications to determine if they should turn off specific memory-intensive
# features that work poorly on low-memory devices.
PRODUCT_PROPERTY_OVERRIDES += ro.config.low_ram="true"
PRODUCT_PROPERTY_OVERRIDES += persist.sys.force_highendgfx="true"
# Disable atlas services on low-ram targets
PRODUCT_PROPERTY_OVERRIDES += config.disable_atlas="true"
# Force Camera Portability API, since newer camera API isn't supported.
PRODUCT_PROPERTY_OVERRIDES += camera2.portability.force_api="1"
