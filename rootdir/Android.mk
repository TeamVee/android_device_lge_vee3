LOCAL_PATH:= $(call my-dir)

# TWRP Hack
$(shell mkdir -p $(OUT)/recovery/root/etc)
$(shell cp $(LOCAL_PATH)/recovery/twrp.fstab $(OUT)/recovery/root/etc/twrp.fstab)
$(shell cp $(LOCAL_PATH)/$(TARGET_DEVICE)/init.recovery.$(TARGET_DEVICE).rc $(OUT)/recovery/root/init.recovery.$(TARGET_DEVICE).rc)
$(shell cp $(LOCAL_PATH)/vee-common/init.recovery.vee-common.sh $(OUT)/recovery/root/init.recovery.$(TARGET_DEVICE).sh)

# Device Rootdir files
$(shell mkdir -p $(TARGET_ROOT_OUT))
$(shell cp $(LOCAL_PATH)/$(TARGET_DEVICE)/init.$(TARGET_DEVICE).rc $(TARGET_ROOT_OUT)/init.$(TARGET_DEVICE).rc)
$(shell cp $(LOCAL_PATH)/$(TARGET_DEVICE)/init.$(TARGET_DEVICE).usb.rc $(TARGET_ROOT_OUT)/init.$(TARGET_DEVICE).usb.rc)

# Common Device Rootdir files
$(shell cp $(LOCAL_PATH)/vee-common/fstab.vee-common $(TARGET_ROOT_OUT)/fstab.$(TARGET_DEVICE))
$(shell cp $(LOCAL_PATH)/vee-common/init.vee-common.sh $(TARGET_ROOT_OUT)/init.$(TARGET_DEVICE)ds.sh)
$(shell cp $(LOCAL_PATH)/vee-common/ueventd.vee-common.rc $(TARGET_ROOT_OUT)/ueventd.$(TARGET_DEVICE).rc)
