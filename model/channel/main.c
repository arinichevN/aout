#include "main.h"

Channel channel_buf[CHANNEL_COUNT];

void channel_RUN(Channel *item);
void channel_OFF(Channel *item);
void channel_FAILURE(Channel *item);

int channel_check(Channel *item);
const char *channel_getErrorStr(Channel *item);
const char *channel_getStateStr(Channel *item);

void channel_INIT(Channel *item) {
	if(item->error_id != ERROR_NO){
		item->control = channel_FAILURE;
		return;
	}
	item->error_id = channel_check(item);
	if(item->error_id != ERROR_NO){
		item->control = channel_FAILURE;
		return;
	}
	item->control = channel_OFF;
	if(item->enable == YES){
#if OUTPUT_MODE == OUT_PWM
		pwm_begin(&item->pwm);
#else
#if OUTPUT_MODE == OUT_SERVO
		servo_begin(&item->servo);
#endif
#endif
		secure_begin(&item->secure);
		item->control = channel_RUN;
	}
	printd("\tINIT: ");printd(item->id); printd(" "); printd(" state=");printd(channel_getStateStr(item));printd(" error=");printd(channel_getErrorStr(item));printd("\n");
}

void channel_RUN(Channel *item) {
	secure_control(&item->secure, &item->goal);
#if OUTPUT_MODE == OUT_PWM
	pwm_control(&item->pwm, item->goal);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_control(&item->servo, item->goal);
#endif
#endif
}

void channel_OFF(Channel *item) {
	;
}

void channel_FAILURE(Channel *item) {
	;
}

int channel_check(Channel *item){
	if(!common_checkBlockStatus(item->enable)){
		return ERROR_BLOCK_STATUS;
	}
	if(!common_checkBlockStatus(item->secure.enable)){
		return ERROR_BLOCK_STATUS;
	}
#if OUTPUT_MODE == OUT_PWM
	if(item->device_kind != DEVICE_KIND_SPWM){
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
	if(item->device_kind != DEVICE_KIND_HPWM){
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
	if(item->control == channel_RUN)			return "RUN";
	else if(item->control == channel_OFF)		return "OFF";
	else if(item->control == channel_INIT)		return "INIT";
	else if(item->control == channel_FAILURE)	return "FAILURE";
	return "?";
}

void channel_setDefaults(Channel *item, size_t ind){
	const ChannelParam *param = &CHANNEL_DEFAULT_PARAMS[ind];
#if OUTPUT_MODE == OUT_PWM
	pwm_setParam (&item->pwm, param->pwm_resolution, param->pwm_period, param->pwm_duty_cycle_min, param->pwm_duty_cycle_max);
	pwm_setPin(&item->pwm, param->pin);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_setParam(&item->servo, param->servo_pw_min, param->servo_pw_max, param->servo_in_min, param->servo_in_max);
	servo_setPin(&item->servo, param->pin);
#endif
#endif
	secure_setParam(&item->secure, param->secure_timeout, param->secure_out, param->secure_enable);
	item->id = param->id;
	item->enable = param->enable;
}

static void channel_setFromNVRAM(Channel *item, size_t ind){
	if(!pmem_getChannel(item, ind)){
		printdln("   failed to get channel");
		item->error_id = ERROR_PMEM_READ;
		return;
	}
}

void channel_setParam(Channel *item, size_t ind, int default_btn){
	if(default_btn == BUTTON_DOWN){
		channel_setDefaults(item, ind);
		pmem_saveChannel(item, ind);
		printd("\tdefault param\n");
	}else{
		channel_setFromNVRAM(item, ind);
		printd("\tNVRAM param\n");
	}
	item->ind = ind;
#if OUTPUT_MODE == OUT_PWM
	item->device_kind = DEVICE_KIND_SPWM;
	item->goal = 0.0;
#else
#if OUTPUT_MODE == OUT_SERVO
	item->device_kind = DEVICE_KIND_HPWM;
	item->goal = item->servo.in_min;
#endif
#endif
}

void channel_begin(Channel *item, size_t ind, int default_btn){
	printd("beginning channel ");printd(ind); printdln(":");
	item->error_id = ERROR_NO;
	channel_setParam(item, ind, default_btn);
	item->control = channel_INIT;
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


void channels_begin(ChannelLList *channels, int default_btn){
	extern Channel channel_buf[CHANNEL_COUNT];
	channels_buildFromArray(channels, channel_buf);
	size_t i = 0;
	FOREACH_CHANNEL(channels){
		channel_begin(channel, i, default_btn); i++;
	}
}

int channel_start(Channel *item){
	printd("starting channel ");printd(item->ind);printdln(":");
	item->enable = YES;
	item->control = channel_INIT;
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
	item->control = channel_OFF;
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
	item->control = channel_INIT;
	return 1;
}

int channels_activeExists(ChannelLList *channels){
	FOREACH_CHANNEL(channels){
		if(channel->control != channel_OFF){
			return 1;
		}
	}
	return 0;
}

void channels_stop(ChannelLList *channels){
	FOREACH_CHANNEL(channels){
		channel_stop(channel);
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
	if(item->control != channel_RUN) return;
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

int CHANNEL_FUN_GET(enable)(Channel *item){return item->enable;}
int CHANNEL_FUN_GET(device_kind)(Channel *item){return item->device_kind;}

int CHANNEL_FUN_GET(pin)(Channel *item){
#if OUTPUT_MODE == OUT_PWM
	return pwm_getPin(&item->pwm);
#else
#if OUTPUT_MODE == OUT_SERVO
	return servo_getPin(&item->servo);
#endif
#endif
}

const char *channel_getSecureStateStr(Channel *item){return secure_getStateStr(&item->secure);}
double CHANNEL_FUN_GET(goal)(Channel *item){return item->goal;}

#if OUTPUT_MODE == OUT_PWM
unsigned long CHANNEL_FUN_GET(pwm_period)(Channel *item){return item->pwm.period;}
unsigned long CHANNEL_FUN_GET(pwm_resolution)(Channel *item){return item->pwm.resolution;}
unsigned long CHANNEL_FUN_GET(pwm_duty_cycle_min)(Channel *item){return item->pwm.duty_cycle_min;}
unsigned long CHANNEL_FUN_GET(pwm_duty_cycle_max)(Channel *item){return item->pwm.duty_cycle_max;}
#else
#if OUTPUT_MODE == OUT_SERVO
unsigned long CHANNEL_FUN_GET(servo_pw_min)(Channel *item){return item->servo.pw_min;}
unsigned long CHANNEL_FUN_GET(servo_pw_max)(Channel *item){return item->servo.pw_max;}
double CHANNEL_FUN_GET(servo_in_min)(Channel *item){return item->servo.in_min;}
double CHANNEL_FUN_GET(servo_in_max)(Channel *item){return item->servo.in_max;}
#endif
#endif

double CHANNEL_FUN_GET(secure_out)(Channel *item){return item->secure.out;}
unsigned long CHANNEL_FUN_GET(secure_timeout)(Channel *item){return item->secure.tmr.interval;}
int CHANNEL_FUN_GET(secure_enable)(Channel *item){return item->secure.enable;}
