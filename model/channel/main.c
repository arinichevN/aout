#include "main.h"

Channel channel_buf[CHANNEL_COUNT];

static void channel_RUN(Channel *item);
static void channel_OFF(Channel *item);
static void channel_FAILURE(Channel *item);

const char *channel_getErrorStr(Channel *item);
const char *channel_getStateStr(Channel *item);

static void channel_INIT(Channel *item) {
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

static void channel_RUN(Channel *item) {
	SECURE_CONTROL(&item->secure, &item->goal);
#if OUTPUT_MODE == OUT_PWM
	PWM_CONTROL(&item->pwm, item->goal);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_control(&item->servo, item->goal);
#endif
#endif
}

static void channel_OFF(Channel *item) {
	;
}

static void channel_FAILURE(Channel *item) {
	;
}

static int channelParam_check(const ChannelParam *item){
	if(!common_checkBlockStatus(item->enable)){
		return ERROR_BLOCK_STATUS;
	}
	if(!common_checkBlockStatus(item->secure_enable)){
		return ERROR_BLOCK_STATUS;
	}
#if OUTPUT_MODE == OUT_PWM
	if(item->pwm_duty_cycle_max < item->pwm_duty_cycle_min){
		return ERROR_PWM;
	}
	if(item->pwm_duty_cycle_max > item->pwm_period){
		return ERROR_PWM;
	}
#else
#if OUTPUT_MODE == OUT_SERVO
	if(item->servo_pw_min > item->servo_pw_max){
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

static int channel_setParam(Channel *item, const ChannelParam *param){
	int r = channelParam_check(param);
	if(r != ERROR_NO){
		return r;
	}
#if OUTPUT_MODE == OUT_PWM
	item->device_kind = DEVICE_KIND_SPWM;
	pwm_setParam (&item->pwm, param->pwm_resolution, param->pwm_period, param->pwm_duty_cycle_min, param->pwm_duty_cycle_max);
	pwm_setPin(&item->pwm, param->pin);
	item->goal = 0.0;
#else
#if OUTPUT_MODE == OUT_SERVO
	item->device_kind = DEVICE_KIND_HPWM;
	servo_setParam(&item->servo, param->servo_pw_min, param->servo_pw_max, param->servo_in_min, param->servo_in_max);
	servo_setPin(&item->servo, param->pin);
	item->goal = item->servo.in_min;
#endif
#endif
	secure_setParam(&item->secure, param->secure_timeout, param->secure_out, param->secure_enable);
	item->id = param->id;
	item->enable = param->enable;
	return ERROR_NO;
}

static int channel_setDefaults(Channel *item, size_t ind){
	const ChannelParam *param = &CHANNEL_DEFAULT_PARAMS[ind];
	int r = channel_setParam(item, param);
	if(r == ERROR_NO){
		pmem_savePChannel(param, ind);
	}
	return r;
}

static int channel_setFromNVRAM(Channel *item, size_t ind){
	ChannelParam param;
	if(!pmem_getPChannel(&param, ind)){
		printdln("   failed to get channel from NVRAM");
		return ERROR_PMEM_READ;
	}
	return channel_setParam(item, &param);
}

static int channel_setParamAlt(Channel *item, size_t ind, int default_btn){
	if(default_btn == BUTTON_DOWN){
		int r = channel_setDefaults(item, ind);
		if(r != ERROR_NO){
			return r;
		}
		printd("\tdefault param\n");
	}else{
		int r = channel_setFromNVRAM(item, ind);
		if(r != ERROR_NO){
			return r;
		}
		printd("\tNVRAM param\n");
	}
	item->ind = ind;
	return ERROR_NO;
}

void channel_begin(Channel *item, size_t ind, int default_btn){
	printd("beginning channel ");printd(ind); printdln(":");
	item->error_id = channel_setParamAlt(item, ind, default_btn);
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

void channel_free(Channel *item){
	;
}

int channel_start(Channel *item){
	if(item->control == channel_OFF || item->control == channel_FAILURE){
		printd("starting channel ");printd(item->ind);printdln(":");
		item->control = channel_INIT;
		item->enable = YES;
		CHANNEL_SAVE_FIELD(enable)
		return 1;
	}
	return 0;
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
	item->error_id = ERROR_NO;
	item->control = channel_OFF;
	item->enable = NO;
	CHANNEL_SAVE_FIELD(enable)
	return 1;
}

int channel_disconnect(Channel *item){
	printd("disconnecting channel ");printdln(item->ind);
#if OUTPUT_MODE == OUT_PWM
	pwm_stop(&item->pwm);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_stop(&item->servo);
#endif
#endif
	secure_stop(&item->secure);
	item->error_id = ERROR_NO;
	item->control = channel_OFF;
	return 1;
}

int channel_reset(Channel *item){
	printd("resetting channel ");printd(item->ind); printdln(":");
#if OUTPUT_MODE == OUT_PWM
	pwm_stop(&item->pwm);
#else
#if OUTPUT_MODE == OUT_SERVO
	servo_stop(&item->servo);
#endif
#endif
	secure_stop(&item->secure);
	channel_free(item);
	channel_begin(item, item->ind, digitalRead(DEFAULT_CONTROL_PIN));
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

int channels_getIdFirst(ChannelLList *channels, int *out){
	int success = 0;
	int f = 0;
	int v;
	FOREACH_CHANNEL(channels){
		if(!f) { v=channel->id; f=1; success=1;}
		if(channel->id < v) v = channel->id;
	}
	*out = v;
	return success;
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
