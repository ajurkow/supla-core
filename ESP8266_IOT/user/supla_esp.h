/*
 ============================================================================
 Name        : supla_esp.h
 Author      : Przemyslaw Zygmunt p.zygmunt@acsoftware.pl [AC SOFTWARE]
 Version     : 1.0
 Copyright   : GPLv2
 ============================================================================
*/

#ifndef SUPLA_ESP_H_
#define SUPLA_ESP_H_

#include "supla-dev/proto.h"

#define DIMMERMODULE
//#define WIFISOCKET
//#define GATEMODULE
//#define WROOM

#ifdef WIFISOCKET
	#define DEVICE_NAME "SUPLA-SOCKET"
	#define TEMPERATURE_CHANNEL 1
#elif defined(GATEMODULE)
	#define DEVICE_NAME "SUPLA-GATE-MODULE"
	#define TEMPERATURE_CHANNEL 4
#elif defined(DIMMERMODULE)
	#define DEVICE_NAME "SUPLA-DIMMER-MODULE"
#endif


#define AP_SSID "SUPLA-ESP8266"

#ifdef WROOM
#define CFG_SECTOR          0xBC
#else
#define CFG_SECTOR          0x3C
#endif

#define SPI_FLASH_SEC_SIZE  4096
#define SERVER_MAXSIZE      100
#define WIFI_SSID_MAXSIZE   32
#define WIFI_PWD_MAXSIZE    64

#define STATE_MAXSIZE       200

#define RECVBUFF_MAXSIZE  1024

#define ACTIVITY_TIMEOUT 10


#define CHANNEL_TYPE

#ifdef WIFISOCKET

#define LED_RED_PORT    13
#define LED_GREEN_PORT  12
#define LED_BLUE_PORT   14
#define RELAY1_PORT      4
#define BTN_PORT         5


#elif defined(GATEMODULE)

#define LED_GREEN_PORT  12
#define LED_BLUE_PORT   14

#define BTN_PORT         5
#define RELAY1_PORT      4
#define RELAY2_PORT      13
#define INPUT_PORT1      12
#define INPUT_PORT2      14

#elif defined(WROOM)

#define LED_GREEN_PORT  5
#define LED_BLUE_PORT   12

#define BTN_PORT         13
#define RELAY1_PORT      4
#define RELAY2_PORT      14
#define INPUT_PORT1      5
#define INPUT_PORT2      12

#elif defined(DIMMERMODULE)
#define LED_BLUE_PORT    2
#define USERSWITCH_PORT  13
#define BTN_PORT         15
#define ZEROCROSS_PORT	 5
#define TRIAC_PORT	 	 4
#endif


#define GPIO_PORT_INIT \
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2); \
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4); \
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5); \
    	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12); \
    	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13); \
    	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14)



#endif /* SUPLA_ESP_H_ */
