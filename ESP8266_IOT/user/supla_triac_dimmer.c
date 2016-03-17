/*
 ============================================================================
 Name        : supla_triac_dimmer.c
 Author      : Andrzej Jurkowski a.jurkowski@onet.eu
 Version     : 0.2
 Copyright   : GPLv2
 ============================================================================
*/

#include <os_type.h>
#include <osapi.h>
#include <gpio.h>
#include <eagle_soc.h>
#include <ets_sys.h>

#include "supla-dev/log.h"
#include "driver/hw_timer.h"

static unsigned char initialized = 0;

void ICACHE_FLASH_ATTR supla_triac_dimmer_init(void)
{


	initialized = 1;
}


void ICACHE_FLASH_ATTR supla_triac_dimmer_zerocross(void)
{
if (!initialized) return;

	hw_timer_arm(1000);	//wylaczenie za 1ms - dlugosc impulsu bramkowego triaka
}
