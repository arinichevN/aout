#include "pmem.h"

#define PMEM_APP_SIZE sizeof(AppConfig)
#define PMEM_CRC_SIZE sizeof(uint8_t)
#define PMEM_CHANNEL_SIZE sizeof(PmemChannel)

#define PMEM_APP_CRC_ADDR 0
#define PMEM_APP_ADDR PMEM_CRC_SIZE

#define PMEM_CHANNEL_ADDR(ind) 		PMEM_CRC_SIZE + PMEM_APP_SIZE + ind * (PMEM_CRC_SIZE + PMEM_CHANNEL_SIZE) + PMEM_CRC_SIZE
#define PMEM_CHANNEL_CRC_ADDR(ind)	PMEM_CRC_SIZE + PMEM_APP_SIZE + ind * (PMEM_CRC_SIZE + PMEM_CHANNEL_SIZE)
#define PMEM_CRC_INI 17

uint8_t pmem_calcCrc(void *data, size_t sz){
	uint8_t crc = PMEM_CRC_INI;
	uint8_t *pt =(uint8_t *) data;
	for (size_t i = 0; i < sz; ++i) {
		crc_update(&crc, pt[i]);
	}
	return crc;
}

uint8_t pmem_calcAppConfigCrc(AppConfig *item){
	return pmem_calcCrc(item, sizeof (AppConfig));
}

uint8_t pmem_calcChannelCrc(PmemChannel *item){
	return pmem_calcCrc(item, sizeof (PmemChannel));
}

void pmem_setAppConfigCrc(AppConfig *item){
	uint8_t crc = pmem_calcAppConfigCrc(item);
	EEPROM.put(PMEM_APP_CRC_ADDR, crc);
}

void pmem_setChannelCrc(PmemChannel *item, size_t ind){
	uint8_t crc = pmem_calcChannelCrc(item);	
	printd("   channel save crc: ");printdln(crc);
	EEPROM.put(PMEM_CHANNEL_CRC_ADDR(ind), crc);
}

int pmem_checkSize(size_t channel_count){
	size_t total_bytes = channel_count * sizeof(PmemChannel) + sizeof(uint8_t) + 1;
	if(EEPROM.length() <= total_bytes){
		return 0;
	}
	return 1;
}

int pmem_checkAppConfigCrc(AppConfig *item){
	uint8_t crc1 = EEPROM[0];
	uint8_t crc2 = pmem_calcAppConfigCrc(item);
	if(crc1 != crc2){
		return 0;
	}
	return 1;
}

int pmem_checkChannelCrc(PmemChannel *item, int ind){
	uint8_t crc1 = EEPROM[PMEM_CHANNEL_CRC_ADDR(ind)];
	printd("   channel get crc: ");printdln(crc1);
	uint8_t crc2 = pmem_calcChannelCrc(item);
	if(crc1 != crc2){
		printd("   bad crc: ");printd(crc1);printd(" ");printdln(crc2);
		return 0;
	}
	return 1;
}


int pmem_hasSpaceForChannel(size_t ind){
	size_t required_bytes = PMEM_CHANNEL_ADDR(ind) + sizeof(PmemChannel) + 1;
	if(EEPROM.length() < required_bytes){
		return 0;
	}
	return 1;
}

int pmem_hasSpaceForAppConfig(){
	size_t required_bytes = PMEM_APP_ADDR + sizeof(AppConfig) + 1;
	if(EEPROM.length() < required_bytes){
		printdln("no space for app_config");
		return 0;
	}
	return 1;
}

void pmem_toChannel(Channel *channel, PmemChannel *pchannel){
	channel->id = pchannel->id;
	channel->enable = pchannel->enable;
	channel->device_id = pchannel->device_id;
#if OUTPUT_MODE == OUT_PWM
	channel->pwm.resolution = pchannel->pwm_resolution;
    channel->pwm.period = pchannel->pwm_period;
    channel->pwm.duty_cycle_min = pchannel->pwm_duty_cycle_min; 
    channel->pwm.duty_cycle_max = pchannel->pwm_duty_cycle_max;
#else
#if OUTPUT_MODE == OUT_SERVO
	channel->servo.pw_min = pchannel->servo_pw_min;
	channel->servo.pw_max = pchannel->servo_pw_max;
	channel->servo.in_min = pchannel->servo_in_min;
	channel->servo.in_max = pchannel->servo_in_max;
#endif
#endif
	channel->secure.out = pchannel->secure_out;
	channel->secure.tmr.interval = pchannel->secure_timeout;
	channel->secure.enable= pchannel->secure_enable;
}

void pmem_fromChannel(PmemChannel *pchannel, Channel *channel){
	pchannel->id = channel->id;
	pchannel->enable = channel->enable;
	pchannel->device_id = channel->device_id;
#if OUTPUT_MODE == OUT_PWM
	pchannel->pwm_resolution = channel->pwm.resolution;
    pchannel->pwm_period = channel->pwm.period;
    pchannel->pwm_duty_cycle_min = channel->pwm.duty_cycle_min;  
    pchannel->pwm_duty_cycle_max = channel->pwm.duty_cycle_max;  
#else
#if OUTPUT_MODE == OUT_SERVO
	pchannel->servo_pw_min = channel->servo.pw_min;
	pchannel->servo_pw_max = channel->servo.pw_max;
	pchannel->servo_in_min = channel->servo.in_min;
	pchannel->servo_in_max = channel->servo.in_max;
#endif
#endif
	pchannel->secure_out = channel->secure.out;  
	pchannel->secure_timeout = channel->secure.tmr.interval;  
	pchannel->secure_enable = channel->secure.enable; 
}

int pmem_getPChannel(PmemChannel *item, size_t ind) {
	if(!pmem_hasSpaceForChannel(ind)){
		return 0;
	}
	PmemChannel t_item;
	EEPROM.get(PMEM_CHANNEL_ADDR(ind), t_item);
	if(!pmem_checkChannelCrc(&t_item, ind)){
		return 0;
	}
	*item = t_item;
	return 1;
}

int pmem_getPChannelForce(PmemChannel *item, size_t ind) {
	if(!pmem_hasSpaceForChannel(ind)){
		return 0;
	}
	EEPROM.get(PMEM_CHANNEL_ADDR(ind), *item);
	return 1;
}

int pmem_getAppConfig(AppConfig *item) {
	if(!pmem_hasSpaceForAppConfig()){
		return 0;
	}
	AppConfig tout;
	EEPROM.get(PMEM_APP_ADDR, tout);
	if(!pmem_checkAppConfigCrc(&tout)){
		return 0;
	}
	*item = tout;
	return 1;
}

int pmem_getAppConfigForce(AppConfig *item) {
	//if(!pmem_hasSpaceForAppConfig()){
	//	return 0;
//	}
	AppConfig temp;
	EEPROM.get(PMEM_APP_ADDR, temp);
	*item = temp;	
	return 1;
}

int pmem_getChannel(Channel *item, size_t ind) {
	if(!pmem_hasSpaceForChannel(ind)){
		printdln("   no space for channel");
		return 0;
	}
	PmemChannel buf;
	EEPROM.get(PMEM_CHANNEL_ADDR(ind), buf);
	if(!pmem_checkChannelCrc(&buf, ind)){
		return 0;
	}
	pmem_toChannel(item, &buf);
	return 1;
}

int pmem_savePChannel(PmemChannel *item, size_t ind){
	printd("(saving channel: ");printd(ind);
	if(!pmem_hasSpaceForChannel(ind)){
		printd("no space) ");
		return 0;
	}
	EEPROM.put(PMEM_CHANNEL_ADDR(ind), *item);
	pmem_setChannelCrc(item, ind);
	printd("done) ");
	return 1;
}
int pmem_saveChannel(Channel *item, size_t ind){
	PmemChannel buf;
	pmem_fromChannel(&buf, item);
	return pmem_savePChannel(&buf, ind);
}


int pmem_saveAppConfig(AppConfig *item){
	printdln("saving device");
	if(!pmem_hasSpaceForAppConfig()){
		printdln("pmem: no space for device");
		return 0;
	}
	EEPROM.put(PMEM_APP_ADDR, *item);
	pmem_setAppConfigCrc(item);
	return 1;
}

PMEMCHANNEL_DEF_GET_FIELD_FUNC(id, int)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(enable, unsigned long)

PMEMCHANNEL_DEF_GET_FIELD_FUNC(secure_enable, int)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(secure_timeout, unsigned long)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(secure_out, double)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(secure_enable, int)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(secure_timeout, unsigned long)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(secure_out, double)

PMEMCHANNEL_DEF_SET_FIELD_FUNC(id, int)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(enable, int)


#if OUTPUT_MODE == OUT_PWM
PMEMCHANNEL_DEF_SET_FIELD_FUNC(pwm_resolution, unsigned long)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(pwm_period, unsigned long)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(pwm_duty_cycle_max, unsigned long)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(pwm_duty_cycle_min, unsigned long)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(pwm_resolution, unsigned long)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(pwm_period, unsigned long)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(pwm_duty_cycle_max, unsigned long)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(pwm_duty_cycle_min, unsigned long)
#else
#if OUTPUT_MODE == OUT_SERVO
PMEMCHANNEL_DEF_SET_FIELD_FUNC(servo_pw_min, unsigned long)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(servo_pw_max, unsigned long)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(servo_in_min, double)
PMEMCHANNEL_DEF_SET_FIELD_FUNC(servo_in_max, double)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(servo_pw_min, unsigned long)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(servo_pw_max, unsigned long)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(servo_in_min, double)
PMEMCHANNEL_DEF_GET_FIELD_FUNC(servo_in_max, double)
#endif
#endif

