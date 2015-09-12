/*
 ============================================================================
 Name        : channel-io.h
 Author      : Przemyslaw Zygmunt p.zygmunt@acsoftware.pl [AC SOFTWARE]
 Version     : 1.0
 Copyright   : GPLv2
 ============================================================================
 */

#ifndef CHANNEL_IO_H_
#define CHANNEL_IO_H_

#include "proto.h"

typedef void (*_func_channelio_valuechanged)(unsigned char number, char value[SUPLA_CHANNELVALUE_SIZE], void *user_data);

char channelio_init(void);
void channelio_free(void);
void channelio_channel_init(void);
int channelio_channel_count(void);
void channelio_set_type(unsigned char number, int type);
void channelio_set_gpio1(unsigned char number, unsigned char gpio1);
void channelio_set_gpio2(unsigned char number, unsigned char gpio2);
void channelio_set_w1(unsigned char number, const char *w1);
char channelio_get_value(unsigned char number, char value[SUPLA_CHANNELVALUE_SIZE]);
char channelio_set_hi_value(unsigned char number, char hi, unsigned int time_ms);
void channelio_channels_to_srd(TDS_SuplaRegisterDevice *srd);

void channelio_setcalback_on_channel_value_changed(_func_channelio_valuechanged on_valuechanged, void *user_data);

#ifdef __SINGLE_THREAD
void channelio_iterate(void);
#endif


#endif /* CHANNEL_IO_H_ */
