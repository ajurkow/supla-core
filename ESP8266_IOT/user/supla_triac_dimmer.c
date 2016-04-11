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
#include "supla_esp.h"
#include "driver/hw_timer.h"

static unsigned char initialized = 0;

void ICACHE_FLASH_ATTR supla_triac_dimmer_zerocross(void);
LOCAL void	triac_dimmer_gpio_intr_handler(void);


void ICACHE_FLASH_ATTR
supla_triac_dimmer_gpio_init(void) {

	ETS_GPIO_INTR_DISABLE();
	
	ETS_GPIO_INTR_ATTACH(triac_dimmer_gpio_intr_handler, 0);
	
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5); 
   	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
   	gpio_pin_intr_state_set(GPIO_ID_PIN(TRIAC_PORT), GPIO_PIN_INTR_DISABLE);

	ETS_GPIO_INTR_ENABLE();

	//BUTTON LOGIN
	gpio_output_set(0, 0, 0, GPIO_ID_PIN(BTN_PORT));
	gpio_register_set(GPIO_PIN_ADDR(BTN_PORT),	GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
											  | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
											  | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
	
	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(BTN_PORT));
	//enable interrupt
	gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_NEGEDGE);
	        

	//ZEROCROSS
	gpio_output_set(0, 0, 0, GPIO_ID_PIN(ZEROCROSS_PORT));
	gpio_register_set(GPIO_PIN_ADDR(ZEROCROSS_PORT),	GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
											  	  	  | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
													  | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
	
	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(ZEROCROSS_PORT));
	//enable interrupt
	gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_ANYEDGE);
	
	//USERSWITCH
	gpio_output_set(0, 0, 0, GPIO_ID_PIN(USERSWITCH_PORT));
	gpio_register_set(GPIO_PIN_ADDR(USERSWITCH_PORT),	GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
												  	  | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
													  | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
		
	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(USERSWITCH_PORT));
	//enable interrupt
	gpio_pin_intr_state_set(GPIO_ID_PIN(USERSWITCH_PORT), GPIO_PIN_INTR_POSEDGE);
		        

	supla_esp_gpio_hi(TRIAC_PORT, 1);

    ETS_GPIO_INTR_ENABLE();

}

LOCAL void
triac_dimmer_gpio_intr_handler(void)
{
    uint8 i;
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

	//tutaj przejme z przerwania od gpio
	//sygnal z Sieci o przejsiu przez ZERO
	//i wywolam odpowiednia funkcje sterujaca triakiem
	if ( gpio_status & BIT(ZEROCROSS_PORT))
	{
		 gpio_pin_intr_state_set(GPIO_ID_PIN(ZEROCROSS_PORT), GPIO_PIN_INTR_DISABLE);
         //clear interrupt status
         GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(ZEROCROSS_PORT));
         
         supla_triac_dimmer_zerocross();
         
         gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_ANYEDGE);
	}
	
	if ( gpio_status & BIT(ZEROCROSS_PORT))
		{
			 gpio_pin_intr_state_set(GPIO_ID_PIN(ZEROCROSS_PORT), GPIO_PIN_INTR_DISABLE);
	         //clear interrupt status
	         GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(ZEROCROSS_PORT));
	         
	         supla_triac_dimmer_zerocross();
	         
	         gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_ANYEDGE);
		}
	
	if ( gpio_status & BIT(ZEROCROSS_PORT))
		{
			 gpio_pin_intr_state_set(GPIO_ID_PIN(ZEROCROSS_PORT), GPIO_PIN_INTR_DISABLE);
	         //clear interrupt status
	         GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(ZEROCROSS_PORT));
	         
	         supla_triac_dimmer_zerocross();
	         
	         gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_ANYEDGE);
		}
	
	
	
}


void ICACHE_FLASH_ATTR supla_triac_dimmer_init(void)
{


	initialized = 1;
}


void ICACHE_FLASH_ATTR supla_triac_dimmer_zerocross(void)
{
if (!initialized) return;

	hw_timer_arm(1000);	//wylaczenie za 1ms - dlugosc impulsu bramkowego triaka
}
