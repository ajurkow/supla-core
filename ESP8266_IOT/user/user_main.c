/*
 ============================================================================
 Name        : user_main.c
 Author      : Przemyslaw Zygmunt p.zygmunt@acsoftware.pl [AC SOFTWARE]
 Version     : 1.0
 Copyright   : GPLv2
 ============================================================================
*/


#include <string.h>
#include <stdio.h>

#include <ip_addr.h>
#include <osapi.h>
#include <mem.h>
#include <ets_sys.h>

#include "supla_esp.h"
#include "supla_esp_cfg.h"
#include "supla_esp_cfgmode.h"
#include "supla_esp_gpio.h"
#include "supla-dev/log.h"


void user_init(void)
{

     supla_log(LOG_DEBUG, "Starting");
     wifi_status_led_uninstall();

     supla_esp_cfg_init();
     supla_esp_devconn_init();
#ifndef DIMMERMODULE
	 supla_esp_gpio_init();
     supla_ds18b20_init();
#endif

#ifdef DIMMERMODULE
	 supla_triac_dimmer_gpio_init();
     supla_triac_dimmer_init();
#endif
     
     if ( supla_esp_cfg.LocationID == 0
    		 || supla_esp_cfg.LocationPwd[0] == 0
    		 || supla_esp_cfg.Server[0] == 0
    		 || supla_esp_cfg.WIFI_PWD[0] == 0
    		 || supla_esp_cfg.WIFI_SSID[0] == 0 ) {

    	 supla_esp_cfgmode_start();
    	 return;
     }

#ifndef DIMMERMODULE
	supla_ds18b20_start();
#endif
	supla_esp_devconn_start();


}

