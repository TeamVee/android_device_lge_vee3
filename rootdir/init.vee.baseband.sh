#!/system/bin/sh
# Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
# Copyright (c) 2014, TeamHackLG. All rights reserved.
# Copyright (c) 2015, TeamVee. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of The Linux Foundation nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# No path is set up at this point so we have to do it here.
PATH=/sbin:/system/sbin:/system/bin:/system/xbin
export PATH

# Set baseband based on modem
basebandcheck=`getprop gsm.version.baseband | grep -o -e "V10" -e "V20"`
case "$basebandcheck" in
	"") setprop gsm.version.baseband `strings /dev/block/mmcblk0p12 | grep -e "-V10.-" -e "-V20.-" | head -1` ;;
esac

# Fix KL of E435 based on baseband
deviceset=`getprop gsm.version.baseband | grep -o -e "E435" | head -1`
case "$deviceset" in
	"E435")
	# ReMount /system to Read-Write
	mount -o rw,remount /system
	# Change KeyLayouts
	busybox sed -i '/key 139   MENU              VIRTUAL/c\key 139   HOME              VIRTUAL' system/usr/keylayout/touch_mcs8000.kl
	busybox sed -i '/key 172   HOME              VIRTUAL/c\key 172   MENU              VIRTUAL' system/usr/keylayout/touch_mcs8000.kl
	# ReMount /system to Read-Only
	mount -o ro,remount /system
	;;
esac

# Set essential configs
echo `getprop ro.serialno` > /sys/class/android_usb/android0/iSerial
echo `getprop ro.product.manufacturer` > /sys/class/android_usb/android0/iManufacturer
echo `getprop ro.product.manufacturer` > /sys/class/android_usb/android0/f_rndis/manufacturer
echo `getprop ro.product.model` > /sys/class/android_usb/android0/iProduct

setprop ro.build.description "`getprop ro.build.product`-`getprop ro.build.type` `getprop ro.build.version.release` `getprop ro.build.id` `getprop ro.build.version.incremental` `getprop ro.build.tags`"
setprop ro.build.fingerprint "`getprop ro.product.manufacturer`/`getprop ro.build.product`/`getprop ro.build.product`:`getprop ro.build.version.release`/`getprop ro.build.id`:`getprop ro.build.type`/`getprop ro.build.tags`"
