/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <stdio.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"

void vendor_load_properties() {
    char gversionbb[92];
    char dversionbb[92];
    char o_gversionbb[92];
    char o_dversionbb[92];
    FILE *fp;

    fp = popen("/system/bin/printf $(/system/bin/strings /dev/block/mmcblk0p12 | /system/bin/egrep -e '-V10' -e '-V20')", "r");
    fgets(gversionbb, sizeof(gversionbb), fp);
    pclose(fp);
    property_set("gsm.version.baseband", gversionbb);

    fp = popen("/system/bin/printf $(/system/bin/getprop gsm.version.baseband | /system/bin/egrep -o -e 'E410' -e 'E411' -e 'E415' -e 'E420' -e 'E425' -e 'E430' -e 'E431' -e 'E435')", "r");
    fgets(dversionbb, sizeof(dversionbb), fp);
    pclose(fp);
    property_set("ro.product.device", dversionbb);
    property_set("ro.product.model", dversionbb);

    if (strstr(dversionbb, "E415") || strstr(dversionbb, "E420") || strstr(dversionbb, "E435")) {
        property_set("persist.radio.multisim.config", "dsds");
        property_set("persist.multisim.config", "dsds");
        property_set("ro.multi.rild", "true");
    };

    //if (strlen(gversionbb) == 0) {
        property_set("gsm.version.baseband", "V10");
        property_set("ro.product.device", "vee3");
        property_set("ro.product.model", "vee3");
    //};

    property_get("gsm.version.baseband", o_gversionbb);
    property_get("ro.product.device", o_dversionbb);

    ERROR("Found %s gsm baseband setting build properties for %s device\n", o_gversionbb, o_dversionbb);
}
