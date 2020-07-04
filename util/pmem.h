#ifndef PMEM_H
#define PMEM_H

#include <EEPROM.h>
#include "crc.h"
#include "../app/config.h"
#include "../app.h"
#include "../model/channel.h"



typedef struct {
	int id;
    int enable;
    int device_id;
#if OUTPUT_MODE == OUT_PWM
	unsigned long pwm_resolution;
    unsigned long pwm_period;
    unsigned long pwm_duty_cycle_min; 
    unsigned long pwm_duty_cycle_max;
#else
#if OUTPUT_MODE == OUT_SERVO
	unsigned long servo_pw_min;
	unsigned long servo_pw_max;
	double servo_in_min;
	double servo_in_max;
#endif
#endif
    double secure_out;
    unsigned long secure_timeout;
    int secure_enable;
} PmemChannel;


extern uint8_t pmem_calcAppConfigCrc(AppConfig *item);

extern uint8_t pmem_calcChannelCrc(size_t ind);

extern void pmem_setAppConfigCrc();

extern void pmem_setChannelCrc(size_t ind);

extern int pmem_checkSize(size_t channel_count);

extern int pmem_checkAppConfigCrc(AppConfig *item);

extern int pmem_checkChannelCrc(PmemChannel *item,int ind);

extern void pmem_printSpace(size_t channel_count);

extern int pmem_hasSpaceForChannel(int ind);

extern int pmem_hasSpaceForAppConfig();

extern void pmem_toChannel(Channel *channel, PmemChannel *pchannel);

extern void pmem_fromChannel(PmemChannel *pchannel, Channel *channel);

extern int pmem_getPChannel(PmemChannel *item, size_t ind) ;

extern int pmem_getPChannelForce(PmemChannel *item, size_t ind) ;

extern int pmem_getAppConfig(AppConfig *item) ;

extern int pmem_getAppConfigForce(AppConfig *item);

extern int pmem_getChannel(Channel *item, size_t ind);

extern int pmem_saveChannel(Channel *item, size_t ind);

extern int pmem_savePChannel(PmemChannel *item, size_t ind);

extern int pmem_saveAppConfig(AppConfig *item);

#define PMEMCHANNEL_GET_FIELD_FUNC(FIELD) pmcgff ## FIELD
#define PMEMCHANNEL_DEF_GET_FIELD_FUNC(FIELD, T) T PMEMCHANNEL_GET_FIELD_FUNC(FIELD)(PmemChannel *item){return item->FIELD;}

#define PMEMCHANNEL_SET_FIELD_FUNC(FIELD) pmcsff ## FIELD
#define PMEMCHANNEL_DEF_SET_FIELD_FUNC(FIELD, T) void PMEMCHANNEL_SET_FIELD_FUNC(FIELD)(PmemChannel *item, T v){item->FIELD = v;}

#endif 
