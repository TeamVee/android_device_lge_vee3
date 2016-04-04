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

/sbin/setprop gsm.version.baseband $(/sbin/strings /dev/block/mmcblk0p12 | /sbin/grep -e "-V10" -e "-V20" | /sbin/head -1)

# Set Variant
/sbin/setprop ro.product.model $(/sbin/getprop gsm.version.baseband | /sbin/grep -o -e "E410" -e "E411" -e "E415" -e "E420" -e "E425" -e "E430" -e "E431" -e "E435" | /sbin/head -1)
/sbin/setprop ro.product.device $(/sbin/getprop ro.product.model)
/sbin/setprop ro.product.manufacturer "LGE"

# Set essential configs
/sbin/echo $(/sbin/getprop ro.serialno) > /sys/class/android_usb/android0/iSerial
/sbin/echo $(/sbin/getprop ro.product.manufacturer) > /sys/class/android_usb/android0/iManufacturer
/sbin/echo $(/sbin/getprop ro.product.manufacturer) > /sys/class/android_usb/android0/f_rndis/manufacturer
/sbin/echo $(/sbin/getprop ro.product.model) > /sys/class/android_usb/android0/iProduct

# Set secondary things
/sbin/setprop ro.build.description "$(/sbin/getprop ro.build.product)-$(/sbin/getprop ro.build.type) $(/sbin/getprop ro.build.version.release) $(/sbin/getprop ro.build.id) $(/sbin/getprop ro.build.version.incremental) $(/sbin/getprop ro.build.tags)"
/sbin/setprop ro.build.fingerprint "$(/sbin/getprop ro.product.manufacturer)/$(/sbin/getprop ro.build.product)/$(/sbin/getprop ro.build.product):$(/sbin/getprop ro.build.version.release)/$(/sbin/getprop ro.build.id):$(/sbin/getprop ro.build.type)/$(/sbin/getprop ro.build.tags)"

# Restart ADBD
/sbin/stop adbd
/sbin/start adbd
