#include "channel.h"

Channel channel_buf[CHANNEL_COUNT];

int channel_check(Channel *item){
	if(!common_checkBlockStatus(item->enable)){
		return ERROR_BLOCK_STATUS;
	}
	if(!common_checkBlockStatus(item->secure.enable)){
		return ERROR_SECURE_BLOCK_STATUS;
	}
#if OUTPUT_MODE == OUT_PWM
	if(item->device_kind !== DEVICE_KIND_SPWM){
		return ERROR_DEVICE_KIND;
	}
	if(item->pwm.duty_cycle_max < item->pwm.duty_cycle_min){
		return ERROR_PWM;
	}
	if(item->pwm.duty_cycle_max > item->pwm.period){
		return ERROR_PWM;
	}
#else
#if OUTPUT_MODE == OUT_SERVO
	if(item->device_kind !== DEVICE_KIND_HPWM){
		return ERROR_DEVICE_KIND;
	}
	if(item->servo.pw_min > item->servo.pw_max){
		return ERROR_PWM;
	}
	if(item->servo.in_min > item->servo.in_max){
		return ERROR_PWM;
	}
#endif
#endif
    return ERROR_NO;
}

const char *channel_getErrorStr(Channel *item){
	return getErrorStr(item->error_id);
}

const char *channel_getStateStr(Channel *item){
	switch(item->state){
		case RUN:return "RUN";
		case OFF:return "OFF";
		case INIT:return "INIT";
		case FAILURE:return "FAILURE";
	}	
	return "?";
}

int channel_getDeviceKind(Channel *item){
	return item->device_kind;
}

void channel_setDefaults(Channel *item, size_t ind){
#if OUTPUT_MODE == OUT_PWM
	pwm_setParam (&item->pwm, DEFAULT_PWM_RESOLUTION, DEFAULT_PWM_PERIOD_MS, DEFAULT_PWM_DUTY_CYCLE_MIN_MS, DEFAULT_PWM_DUTY_CYCLE_MAX_MS);
	item->device_kind = DEVICE_KIND_SPWM;
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_setParam(&item->servo, DEFAULT_SERVO_PW_MIN, DEFAULT_SERVO_PW_MAX, DEFAULT_SERVO_IN_MIN, DEFAULT_SERVO_IN_MAX);
	item->device_kind = DEVICE_KIND_HPWM;
#endif
#endif
	secure_setParam(&item->secure, DEFAULT_SECURE_TIMEOUT_MS, DEFAULT_SECURE_OUT, DEFAULT_SECURE_ENABLE);
	item->id = ind + DEFAULT_CHANNEL_FIRST_ID;
	item->ind = ind;
	item->goal = DEFAULT_CHANNEL_GOAL;
	item->enable = DEFAULT_CHANNEL_ENABLE;
}

void channel_setStaticParam(Channel *item, int pin){
#if OUTPUT_MODE == OUT_PWM
	pwm_setPin(&item->pwm, pin);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_setPin(&item->servo, pin);
#endif
#endif
}

static void channel_setFromNVRAM(Channel *item, size_t ind){
	if(!pmem_getChannel(item, ind)){
		printdln("   failed to get channel");
		item->error_id = ERROR_PMEM_READ;
		return;
	}
#if OUTPUT_MODE == OUT_PWM
	item->goal = 0.0;
	item->device_kind = DEVICE_KIND_SPWM;
#else
#if OUTPUT_MODE == OUT_SERVO
	item->goal = item->servo.in_min;
	item->device_kind = DEVICE_KIND_HPWM;
#endif
#endif
	item->ind = ind;
}



void channel_begin(Channel *item, size_t ind, int default_btn){
	printd("beginning channel ");printd(ind); printdln(":");
	if(default_btn == BUTTON_DOWN){
		channel_setDefaults(item, ind);
		pmem_saveChannel(item, ind);
		printd("\tdefault param\n");
	}else{
		channel_setFromNVRAM(item, ind);
		printd("\tNVRAM param\n");
	}
	item->state = INIT;
	printd("\tid: ");printdln(item->id);
	printd("\n");
}

void channels_buildFromArray(ChannelLList *channels, Channel arr[]){
	if(CHANNEL_COUNT <= 0) return;
	channels->length = CHANNEL_COUNT;
	channels->top = &arr[0];
	channels->last = &arr[CHANNEL_COUNT - 1];
	for(size_t i = 0;i<CHANNEL_COUNT;i++){
		arr[i].next = NULL;
	}
	for(size_t i = 0; i<CHANNEL_COUNT-1; i++){
		arr[i].next = &arr[i+1];
	}
}

#define SET_CHANNEL_PIN(PIN) if(chn != NULL){channel_setStaticParam(chn, PIN);chn = chn->next;} else {printdln("call SET_CHANNEL_PIN for each channel"); return 0;}
int channels_begin(ChannelLList *channels, int default_btn){
	extern Channel channel_buf[CHANNEL_COUNT];
	channels_buildFromArray(channels, channel_buf);
	Channel *chn = channels->top;
	/*
	 * -user_config:
	 * call
	 * SET_CHANNEL_STATIC_PARAM(pin)
	 * for each channel:
	 */
	SET_CHANNEL_STATIC_PARAM(9)
	SET_CHANNEL_STATIC_PARAM(10)
	SET_CHANNEL_STATIC_PARAM(11)
	
	if(chn != NULL){
		printd("number of calles of SET_CHANNEL_STATIC_PARAM() should be equal to CHANNEL_COUNT");
		return 0;
	}
	size_t i = 0;
	FOREACH_CHANNEL(channels)
		channel_begin(channel, i, default_btn); i++;
		if(channel->error_id != ERROR_NO) return 0;
	}
	return 1;
}

int channel_start(Channel *item){
	printd("starting channel ");printd(item->ind);printdln(":");
	item->enable = YES;
	item->state = INIT;
	PmemChannel pchannel;
	if(pmem_getPChannel(&pchannel, item->ind)){
		pchannel.enable = item->enable;
		pmem_savePChannel(&pchannel, item->ind);
	}
	return 1;
}

int channel_stop(Channel *item){
	printd("stopping channel ");printdln(item->ind); 
#if OUTPUT_MODE == OUT_PWM
	pwm_stop(&item->pwm);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_stop(&item->servo);
#endif
#endif
	secure_stop(&item->secure);
	item->enable = NO;
	item->state = INIT;
	PmemChannel pchannel;
	if(pmem_getPChannel(&pchannel, item->ind)){
		pchannel.enable = item->enable;
		pmem_savePChannel(&pchannel, item->ind);
	}
	return 1;
}

int channel_reload(Channel *item){
	printd("reloading channel ");printd(item->ind); printdln(":");
	channel_setFromNVRAM(item, item->ind);
	item->state = INIT;
	return 1;
}

int channels_activeExists(ChannelLList *channels){
	FOREACH_CHANNEL(channels)
		if(channel->state != OFF){
			return 1;
		}
	}
	return 0;
}

void channels_stop(ChannelLList *channels){
	FOREACH_CHANNEL(channels)
		channel_stop(channel);
	}
}

void channel_control(Channel *item) {
	switch(item->state){
		case RUN:
			secure_control(&item->secure, &item->goal);
#if OUTPUT_MODE == OUT_PWM
			pwm_control(&item->pwm, item->goal);
#else
#if OUTPUT_MODE == OUT_SERVO
			servo_control(&item->servo, item->goal);
#endif
#endif
			break;
		case OFF:
			break;
		case FAILURE:
			break;
		case INIT:
			item->error_id = ERROR_NO;
			item->error_id = channel_check(item);
		    if(item->error_id != ERROR_NO){
		        item->state = FAILURE;
		        break;
		    }
			item->state = OFF;
			if(item->enable == YES){
#if OUTPUT_MODE == OUT_PWM
				pwm_begin(&item->pwm);
#else
#if OUTPUT_MODE == OUT_SERVO
				servo_begin(&item->servo);
#endif
#endif
				secure_begin(&item->secure);
				item->state = RUN;
			}else{
				item->state = OFF;
			}
			printd("\tINIT: ");printd(item->id); printd(" "); printd(" state=");printd(channel_getStateStr(item));printd(" error=");printd(channel_getErrorStr(item));printd("\n");
			break;
		default:
			break;
	}
}

Channel * channel_getById(int id, Channel *channels, size_t channel_count){
	for(size_t i = 0; i< channel_count;i++){
		Channel *channel = &channels[i];
		if(channel->id == id){
			return channel;
		}
	}
	return NULL;
}

void channel_setGoal(Channel *item, double v){
	if(item->state != RUN) return;
#if OUTPUT_MODE == OUT_PWM
	pwm_normalizeInput(&item->pwm, &v);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_normalizeInput(&item->servo, &v);
#endif
#endif
	item->goal = v;
	secure_touch(&item->secure);
}

const char *channel_getSecureStateStr(Channel *item){return secure_getStateStr(&item->secure);}
double channel_getGoal(Channel *item){return item->goal;}

#if OUTPUT_MODE == OUT_PWM
unsigned long channel_getPWMPeriod(Channel *item){return item->pwm.period;}
unsigned long channel_getPWMResolution(Channel *item){return item->pwm.resolution;}
unsigned long channel_getPWMDCMin(Channel *item){return item->pwm.duty_cycle_min;}
unsigned long channel_getPWMDCMax(Channel *item){return item->pwm.duty_cycle_max;}
#else
#if OUTPUT_MODE == OUT_SERVO
unsigned long channel_getServoPWMin(Channel *item){return item->servo.pw_min;}
unsigned long channel_getServoPWMax(Channel *item){return item->servo.pw_max;}
double channel_getServoInMin(Channel *item){return item->servo.in_min;}
double channel_getServoInMax(Channel *item){return item->servo.in_max;}
#endif
#endif



double channel_getSecureOut(Channel *item){return item->secure.out;}
unsigned long channel_getSecureTimeout(Channel *item){return item->secure.tmr.interval;}
int channel_getSecureEnable(Channel *item){return item->secure.enable;}
int channel_getEnable(Channel *item){return item->enable;}
