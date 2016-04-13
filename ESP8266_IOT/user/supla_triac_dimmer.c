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

static unsigned char 	initialized = 0;
static unsigned char 	fire;
static ETSTimer 		LEDtim,ONtim,OFFtim,delaytim;
static unsigned char	Brightness=0;
static unsigned char	MonoSwitch_dir;
static unsigned char    direction=1;


LOCAL void	triac_dimmer_gpio_intr_handler(void);
static void ICACHE_FLASH_ATTR supla_triac_dimmer_TimerCb(void *arg);


void ICACHE_FLASH_ATTR
supla_triac_dimmer_gpio_init(void) {

	ETS_GPIO_INTR_DISABLE();
	
	ETS_GPIO_INTR_ATTACH(triac_dimmer_gpio_intr_handler, 0);
	
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5); 
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
   	
   	gpio_pin_intr_state_set(GPIO_ID_PIN(TRIAC_PORT), GPIO_PIN_INTR_DISABLE);
	gpio_pin_intr_state_set(GPIO_ID_PIN(LED_BLUE_PORT), GPIO_PIN_INTR_DISABLE);


	//BUTTON LOGIN
	gpio_output_set(0, 0, 0, GPIO_ID_PIN(BTN_PORT));
	gpio_register_set(GPIO_ID_PIN(BTN_PORT),	GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
											  | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
											  | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
	
	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(BTN_PORT));
	//enable interrupt
	gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_POSEDGE);
	        

	//ZEROCROSS
	gpio_output_set(0, 0, 0, GPIO_ID_PIN(ZEROCROSS_PORT));
	gpio_register_set(GPIO_ID_PIN(ZEROCROSS_PORT),	GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
											  	  	  | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
													  | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
	
	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(ZEROCROSS_PORT));
	//enable interrupt
	gpio_pin_intr_state_set(GPIO_ID_PIN(ZEROCROSS_PORT), GPIO_PIN_INTR_ANYEDGE);
	
	//USERSWITCH
	gpio_output_set(0, 0, 0, GPIO_ID_PIN(USERSWITCH_PORT));
	gpio_register_set(GPIO_ID_PIN(USERSWITCH_PORT),	GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
												  	  | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
													  | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
		
	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(USERSWITCH_PORT));
	//enable interrupt
	gpio_pin_intr_state_set(GPIO_ID_PIN(USERSWITCH_PORT), GPIO_PIN_INTR_ANYEDGE);
		        

	GPIO_OUTPUT_SET(TRIAC_PORT,1);
    ETS_GPIO_INTR_ENABLE();

}



LOCAL void
triac_dimmer_gpio_intr_handler(void)
{
    uint16 i;
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

	//sygnal z Sieci o przejsiu przez ZERO
	if ( gpio_status & BIT(ZEROCROSS_PORT))
	{
		 gpio_pin_intr_state_set(GPIO_ID_PIN(ZEROCROSS_PORT), GPIO_PIN_INTR_DISABLE);
         
		 if ((initialized)&&(Brightness>=100))
		 {
			 GPIO_OUTPUT_SET(TRIAC_PORT,0); //zalaczony na 100%
		 }
		 else
		 {
		 if (initialized)
        	{
        	 GPIO_OUTPUT_SET(TRIAC_PORT,1);	//dajemy wysoki na bramke, wylaczony tranzystor,wylaczony triak 
        	 if(Brightness>0)
        	 {
        		 //przeliczenie jasnosci liniowo bez uwzglednienia sinusa
        		 //min 5% maks 95%
        		 if(Brightness<=90) i=9400;
        		 if(Brightness<=80) i=8700;
        		 if(Brightness<=70) i=8100;
        		 if(Brightness<=60) i=7400;
        		 if(Brightness<=50) i=6700;
        		 if(Brightness<=40) i=5900;
        		 if(Brightness<=30) i=5100;
        		 if(Brightness<=20) i=4100;
        		 if(Brightness<=10) i=2900;
        		 
        		 i = 10000 - i;			//to jest wartosc dimmera w mikrosekundach min 500 max 9500 (0.5ms do 9.5ms
        		 
        		 i = i-270; //okolo 270us spoznia sie zbocze zerocross
        		 fire=1;		
        		 hw_timer_arm(i);		//opoznione zalaczenie	         		 
        	 }
        	 else
        	 {
        		 //OFF
        		 fire = 0;
        		 hw_timer_arm(10000);
        	 }
        	}
        }
         //clear interrupt status and enable intr
         GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(ZEROCROSS_PORT));
         gpio_pin_intr_state_set(GPIO_ID_PIN(ZEROCROSS_PORT), GPIO_PIN_INTR_ANYEDGE);
	}
	
	
	if ( gpio_status & BIT(BTN_PORT))
	{
			 gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_DISABLE);
			 
			 
			 
			 //clear interrupt status and enable intr
	         GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(BTN_PORT));
	         gpio_pin_intr_state_set(GPIO_ID_PIN(BTN_PORT), GPIO_PIN_INTR_POSEDGE);
	}
	
	if ( gpio_status & BIT(USERSWITCH_PORT))
		{
			 gpio_pin_intr_state_set(GPIO_ID_PIN(USERSWITCH_PORT), GPIO_PIN_INTR_DISABLE);
			 
			 
			 //sprawdzam stan portu - wysoki zalaczenie, niski wylaczenie
			 if(GPIO_INPUT_GET(USERSWITCH_PORT) ^ MonoSwitch_dir)
			 {
				 if(direction)
				 {
					 if(Brightness<100)Brightness+=10;
					 os_timer_arm(&ONtim,500,true);
					 os_timer_disarm(&OFFtim);
				 }
				 else
				 {
					 if(Brightness>0)Brightness-=10;
					 os_timer_arm(&OFFtim,500,true);
					 os_timer_disarm(&ONtim);
				 }	 
			 }
			 else
			 {
				 os_timer_disarm(&OFFtim);
				 os_timer_disarm(&ONtim);
				 direction = 1 - direction;
				 MonoSwitch_dir = GPIO_INPUT_GET(USERSWITCH_PORT);
				 //os_timer_disarm(&ONtim);
			 }
			 //clear interrupt status and enable intr
	         GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(USERSWITCH_PORT));
	         gpio_pin_intr_state_set(GPIO_ID_PIN(USERSWITCH_PORT), GPIO_PIN_INTR_ANYEDGE);
		}
	
	
	
}

static void ICACHE_FLASH_ATTR supla_triac_dimmer_TimerCb(void *arg) 
{
	if(fire)
	{
		GPIO_OUTPUT_SET(TRIAC_PORT,0);
		hw_timer_arm(500);	//wylaczenie za 1ms - dlugosc impulsu bramkowego triaka
		fire = 0;
	}
	else
	{
		GPIO_OUTPUT_SET(TRIAC_PORT,1);	//wylaczenie triaka
		hw_timer_arm(100000);
	}
}
LOCAL void OFFproc(void)
{
	if(Brightness>100) Brightness=100;
	if(Brightness>0) Brightness-=10;
	else 
	{
		MonoSwitch_dir = GPIO_INPUT_GET(USERSWITCH_PORT);
		direction = 1;
		os_timer_disarm(&OFFtim);
	}
}

LOCAL void ONproc(void)
{
	if(Brightness<120) Brightness+=10;
	else 
	{
		Brightness=100;
		MonoSwitch_dir = GPIO_INPUT_GET(USERSWITCH_PORT);
		direction = 0;
		os_timer_disarm(&ONtim);
	}
}

LOCAL void AliveLED(void)
{
	static unsigned char LED=0;
	
	if(!direction)	GPIO_OUTPUT_SET(LED_BLUE_PORT,1);
	else			GPIO_OUTPUT_SET(LED_BLUE_PORT,0);
	LED = 1 - LED;
}

LOCAL void Initproc(void)
{
	os_timer_disarm(&delaytim);
	MonoSwitch_dir = GPIO_INPUT_GET(USERSWITCH_PORT);	//stan mono klawisza po zalaczeniu zasilania
	fire = 0;
	initialized = 1;	
}

void ICACHE_FLASH_ATTR supla_triac_dimmer_init(void)
{

 	os_timer_disarm(&LEDtim);
	os_timer_setfn(&LEDtim, AliveLED, NULL);
	os_timer_arm(&LEDtim,100,true);
	
	os_timer_disarm(&OFFtim);
	os_timer_setfn(&OFFtim, OFFproc, NULL);
		
	os_timer_disarm(&ONtim);
	os_timer_setfn(&ONtim, ONproc, NULL);
	
	os_timer_disarm(&delaytim);
	os_timer_setfn(&delaytim, Initproc, NULL);
	os_timer_arm(&delaytim, 500,false);
	
	hw_timer_init(FRC1_SOURCE, 0);
	hw_timer_set_func(supla_triac_dimmer_TimerCb);
}


