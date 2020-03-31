
extern App app;
extern AppSerial serials[];
extern ChannelLList channels;
extern void print_var(HardwareSerial *serial);

#include "../acp/command/app.h"
#include "../acp/command/channel.h"
#include "../acp/command/common.h"
#include "../acp/command/pwm.h"
#include "../acp/command/servo.h"
#include "../acp/command/secure.h"
#include "../app/main.h"

Channel *srvc_getChannel(ACPL *item){
	int v;
	if(acp_packGetCellI(item->buf, 1, &v)){
		FOREACH_CHANNEL(&channels)
			if(channel->id == v){
				return channel;
			}
		}
		printdln("\tchannel not found");
		return NULL;

	}
	printdln("\tfailed to get channel_id");
	return NULL;
}

int srvc_forThisApp(ACPL *item){
	int v;
	if(acp_packGetCellI(item->buf, 1, &v)){
		if(v == app.id){
			return 1;
		}else{
			printdln("\tnot for this app");
			return 0;
		}
	}
	printdln("\tfailed to get app_id");
	return 0;
}

void acnf_getFTS (ACPL *item, HardwareSerial *serial){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	FTS fts;
	fts.value = channel->goal;
	fts.state = 1;
	fts.tm = getCurrentTs();
	if(acp_buildPackIFTS(item->buf, ACP_BUF_MAX_LENGTH, channel->id, &fts)){
		ACPL_PREP_SEND
	}else{
		ACPL_RESET
	}
}

void acnf_getExists(ACPL *item, HardwareSerial *serial){
	int v;
	if(acp_packGetCellI(item->buf, 1, &v)){
		FOREACH_CHANNEL(&channels)
			if(v == channel->id){
				goto success;
			}
		}
		if(v == app.id)	goto success;
	}
	ACPL_RESET
	return;
	success:
	if(acp_buildPackII(item->buf, ACP_BUF_MAX_LENGTH, v, 1)){ACPL_PREP_SEND}else{ACPL_RESET}
}

void srvc_setChannelParamUl(ACPL *item, void (*func)(PmemChannel *, unsigned long )){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	unsigned long v;
	if(acp_packGetCellUl(item->buf, 2, &v)){
		PmemChannel pchannel;
		if(pmem_getPChannelForce(&pchannel, channel->ind)){
			func(&pchannel, v);
			pmem_savePChannel(&pchannel, channel->ind);
		}
	}
	ACPL_RESET
}

void srvc_setChannelParamI(ACPL *item, void (*func)(PmemChannel *, int )){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	int v;
	if(acp_packGetCellI(item->buf, 2, &v)){
		PmemChannel pchannel;
		if(pmem_getPChannel(&pchannel, channel->ind)){
			func(&pchannel, v);
			pmem_savePChannel(&pchannel, channel->ind);
		}
	}
	ACPL_RESET
}

void srvc_setChannelParamF(ACPL *item, void (*func)(PmemChannel *, double )){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	double v;
	if(acp_packGetCellF(item->buf, 2, &v)){
		PmemChannel pchannel;
		if(pmem_getPChannel(&pchannel, channel->ind)){
			func(&pchannel, v);
			pmem_savePChannel(&pchannel, channel->ind);
		}
	}
	ACPL_RESET
}

void acnf_setIdFirst(ACPL *item, HardwareSerial *serial){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	int v;
	if(acp_packGetCellI(item->buf, 2, &v)){
		FOREACH_CHANNEL(&channels)
			PmemChannel pchannel;
			if(pmem_getPChannel(&pchannel, channel->ind)){
				pchannel.id = v; v++;
				pmem_savePChannel(&pchannel, channel->ind);
			}
		}
	}
	ACPL_RESET
}

void acnf_setGoal(ACPL *item, HardwareSerial *serial){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	double v;
	if(acp_packGetCellF(item->buf, 2, &v)){
		printd("(set goal:");printd(v);printd(" ");printd((unsigned long) v);printd(") ");
		channel_setGoal(channel, v);
	}
	ACPL_RESET
}


void fgetChannelPmemParamUl(ACPL *item, unsigned long (*getfunc)(PmemChannel *)){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	PmemChannel pdata;
	int f = 0;unsigned long out = 0;
	if(pmem_getPChannel(&pdata, channel->ind)){
		out = getfunc(&pdata);
		f = 1;
	}
	int r = acp_buildPackIUlI(item->buf, ACP_BUF_MAX_LENGTH, channel->id, out, f);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void fgetChannelPmemParamI(ACPL *item, int (*getfunc)(PmemChannel *)){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	PmemChannel pdata;
	int f = 0;int out = 0;
	if(pmem_getPChannel(&pdata, channel->ind)){
		out = getfunc(&pdata);
		f = 1;
	}
	int r = acp_buildPackIII(item->buf, ACP_BUF_MAX_LENGTH, channel->id, out, f);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}
void fgetChannelPmemParamF(ACPL *item, double (*getfunc)(PmemChannel *)){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	PmemChannel pdata;
	int f = 0;double out = 0;
	if(pmem_getPChannel(&pdata, channel->ind)){
		out = getfunc(&pdata);
		f = 1;
	}
	int r = acp_buildPackIFI(item->buf, ACP_BUF_MAX_LENGTH, channel->id, out, f);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void fgetAppPmemParamI(ACPL *item, int app_id, int (*getfunc)(AppConfig *)){
	AppConfig pdata;
	int f = 0;int out = 0;
	if(pmem_getAppConfig(&pdata)){
		out = getfunc(&pdata);
		f = 1;
	}
	int r = acp_buildPackIII(item->buf, ACP_BUF_MAX_LENGTH, app_id, out, f);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}


void srvc_getChannelParamUl(ACPL *item, unsigned long (*getfunc)(Channel *)){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	unsigned long out = getfunc(channel);
	int r = acp_buildPackIUl(item->buf, ACP_BUF_MAX_LENGTH, channel->id, out);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void srvc_getChannelParamI(ACPL *item, int (*getfunc)(Channel *)){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	int out = getfunc(channel);
	int r = acp_buildPackII(item->buf, ACP_BUF_MAX_LENGTH, channel->id, out);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void srvc_getChannelParamF(ACPL *item, double (*getfunc)(Channel *)){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	double out = getfunc(channel);
	int r = acp_buildPackIF(item->buf, ACP_BUF_MAX_LENGTH, channel->id, out);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void srvc_getChannelParamStr(ACPL *item, const char *(*getfunc)(Channel *)){
	Channel *channel = srvc_getChannel(item);
	if(channel == NULL) {ACPL_RESET return;}
	const char *out = getfunc(channel);
	int r = acp_buildPackIS(item->buf, ACP_BUF_MAX_LENGTH, channel->id, out);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void acnf_getIdFirst(ACPL *item, HardwareSerial *serial){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	int out;
	int success = 0;
	FOREACH_CHANNEL(&channels)
		PmemChannel pchannel;
		if(pmem_getPChannel(&pchannel, channel->ind)){
			if(channel == channels.top){out = pchannel.id; success = 1;}
			if(pchannel.id < out){
				out = pchannel.id;
			}
		}else{
			success = 0;
			break;
		}
	}
	int r = acp_buildPackIII(item->buf, ACP_BUF_MAX_LENGTH, app.id, out, success);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void acnf_setId(ACPL *item, HardwareSerial *serial){srvc_setChannelParamI(item, &PMEMCHANNEL_SET_FIELD_FUNC(id));}

void acnf_getGoal(ACPL *item, HardwareSerial *serial){srvc_getChannelParamF(item, &channel_getGoal);}


#if OUTPUT_MODE == OUT_PWM
void acnf_setPWMResolution(ACPL *item, HardwareSerial *serial){srvc_setChannelParamUl(item, &PMEMCHANNEL_SET_FIELD_FUNC(pwm_resolution));}
void acnf_setPWMPeriod(ACPL *item, HardwareSerial *serial){srvc_setChannelParamUl(item, &PMEMCHANNEL_SET_FIELD_FUNC(pwm_period));}
void acnf_setPWMDCMin(ACPL *item, HardwareSerial *serial){srvc_setChannelParamUl(item, &PMEMCHANNEL_SET_FIELD_FUNC(pwm_duty_cycle_min));}
void acnf_setPWMDCMax(ACPL *item, HardwareSerial *serial){srvc_setChannelParamUl(item, &PMEMCHANNEL_SET_FIELD_FUNC(pwm_duty_cycle_max));}
void acnf_getPWMResolution(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamUl(item, &PMEMCHANNEL_GET_FIELD_FUNC(pwm_resolution));}
void acnf_getPWMPeriod(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamUl(item, &PMEMCHANNEL_GET_FIELD_FUNC(pwm_period));}
void acnf_getPWMDCMin(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamUl(item, &PMEMCHANNEL_GET_FIELD_FUNC(pwm_duty_cycle_min));}
void acnf_getPWMDCMax(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamUl(item, &PMEMCHANNEL_GET_FIELD_FUNC(pwm_duty_cycle_max));}
void acnf_getrPWMResolution(ACPL *item, HardwareSerial *serial){srvc_getChannelParamUl(item, &channel_getPWMResolution);}
void acnf_getrPWMPeriod(ACPL *item, HardwareSerial *serial){srvc_getChannelParamUl(item, &channel_getPWMPeriod);}
void acnf_getrPWMDCMin(ACPL *item, HardwareSerial *serial){srvc_getChannelParamUl(item, &channel_getPWMDCMin);}
void acnf_getrPWMDCMax(ACPL *item, HardwareSerial *serial){srvc_getChannelParamUl(item, &channel_getPWMDCMax);}
#else
#if OUTPUT_MODE == OUT_SERVO
void acnf_setServoPWMin(ACPL *item, HardwareSerial *serial){srvc_setChannelParamUl(item, &PMEMCHANNEL_SET_FIELD_FUNC(servo_pw_min));}
void acnf_setServoPWMax(ACPL *item, HardwareSerial *serial){srvc_setChannelParamUl(item, &PMEMCHANNEL_SET_FIELD_FUNC(servo_pw_max));}
void acnf_setServoInMin(ACPL *item, HardwareSerial *serial){srvc_setChannelParamF(item, &PMEMCHANNEL_SET_FIELD_FUNC(servo_in_min));}
void acnf_setServoInMax(ACPL *item, HardwareSerial *serial){srvc_setChannelParamF(item, &PMEMCHANNEL_SET_FIELD_FUNC(servo_in_max));}
void acnf_getServoPWMin(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamUl(item, &PMEMCHANNEL_GET_FIELD_FUNC(servo_pw_min));}
void acnf_getServoPWMax(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamUl(item, &PMEMCHANNEL_GET_FIELD_FUNC(servo_pw_max));}
void acnf_getServoInMin(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamF(item, &PMEMCHANNEL_GET_FIELD_FUNC(servo_in_min));}
void acnf_getServoInMax(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamF(item, &PMEMCHANNEL_GET_FIELD_FUNC(servo_in_max));}
void acnf_getrServoPWMin(ACPL *item, HardwareSerial *serial){srvc_getChannelParamUl(item, &channel_getServoPWMin);}
void acnf_getrServoPWMax(ACPL *item, HardwareSerial *serial){srvc_getChannelParamUl(item, &channel_getServoPWMax);}
void acnf_getrServoInMin(ACPL *item, HardwareSerial *serial){srvc_getChannelParamF(item, &channel_getServoInMin);}
void acnf_getrServoInMax(ACPL *item, HardwareSerial *serial){srvc_getChannelParamF(item, &channel_getServoInMax);}
#endif
#endif



void acnf_setSecureOut(ACPL *item, HardwareSerial *serial){srvc_setChannelParamF(item, &PMEMCHANNEL_SET_FIELD_FUNC(secure_out));}
void acnf_setSecureTimeout(ACPL *item, HardwareSerial *serial){srvc_setChannelParamUl(item, &PMEMCHANNEL_SET_FIELD_FUNC(secure_timeout));}
void acnf_setSecureEnable(ACPL *item, HardwareSerial *serial){srvc_setChannelParamI(item, &PMEMCHANNEL_SET_FIELD_FUNC(secure_enable));}
void acnf_getSecureOut(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamF(item, &PMEMCHANNEL_GET_FIELD_FUNC(secure_out));}
void acnf_getSecureTimeout(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamUl(item, &PMEMCHANNEL_GET_FIELD_FUNC(secure_timeout));}
void acnf_getSecureEnable(ACPL *item, HardwareSerial *serial){fgetChannelPmemParamI(item, &PMEMCHANNEL_GET_FIELD_FUNC(secure_enable));}
void acnf_getrSecureOut(ACPL *item, HardwareSerial *serial){srvc_getChannelParamF(item, &channel_getSecureOut);}
void acnf_getrSecureTimeout(ACPL *item, HardwareSerial *serial){srvc_getChannelParamUl(item, &channel_getSecureTimeout);}
void acnf_getrSecureEnable(ACPL *item, HardwareSerial *serial){srvc_getChannelParamI(item, &channel_getSecureEnable);}

void acnf_getrGoal(ACPL *item, HardwareSerial *serial){srvc_getChannelParamF(item, &channel_getGoal);}

void acnf_getErrorStr(ACPL *item, HardwareSerial *serial){srvc_getChannelParamStr(item, &channel_getErrorStr);}
void acnf_getStateStr(ACPL *item, HardwareSerial *serial){srvc_getChannelParamStr(item, &channel_getStateStr);}
void acnf_getSecureStateStr(ACPL *item, HardwareSerial *serial){srvc_getChannelParamStr(item, &channel_getSecureStateStr);}
void acnf_getrEnable(ACPL *item, HardwareSerial *serial){srvc_getChannelParamI(item, &channel_getEnable);}


void acnf_getAppVar(ACPL *item, HardwareSerial *serial){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	print_var(serial);
	ACPL_RESET
}

void acnf_start(ACPL *item, HardwareSerial *serial){
	Channel *channel = srvc_getChannel(item);
	if(channel!=NULL){
		channel_start(channel);
	}
	ACPL_RESET
}

void acnf_stop(ACPL *item, HardwareSerial *serial){
	Channel *channel = srvc_getChannel(item);
	if(channel!=NULL){
		channel_stop(channel);
	}
	ACPL_RESET
}

void acnf_reload(ACPL *item, HardwareSerial *serial){
	Channel *channel = srvc_getChannel(item);
	if(channel!=NULL){
		channel_reload(channel);
	}
	ACPL_RESET
}

//void srvc_getrAppConfigField(ACPL *item, int (*getfunc)(AppConfig *)){
	//if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	//int v = getfunc(&config);
	//int r = acp_buildPackI(item->buf, ACP_BUF_MAX_LENGTH, v);
	//if(r){ACPL_PREP_SEND}else{ACPL_RESET}
//}

void srvc_getrAppFieldSF(ACPL *item, const char * (*getfunc)(App *)){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	extern App app;
	const char *v = getfunc(&app);
	int r = acp_buildPackIS(item->buf, ACP_BUF_MAX_LENGTH, app.id, v);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void srvc_getAppConfigField(ACPL *item, int (*getfunc)(AppConfig *)){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	AppConfig conf;
	int v = 0;
	int result = 0;
	if(pmem_getAppConfig(&conf)){
		v = getfunc(&conf);
		result = 1;
	}
	int r = acp_buildPackII(item->buf, ACP_BUF_MAX_LENGTH, v, result);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void srvc_getAppConfigFieldBr(ACPL *item, int (*getfunc)(AppConfig *)){
	AppConfig conf;
	int v = 0;
	int result = 0;
	if(pmem_getAppConfig(&conf)){
		v = getfunc(&conf);
		result = 1;
	}
	int r = acp_buildPackII(item->buf, ACP_BUF_MAX_LENGTH, v, result);
	if(r){ACPL_PREP_SEND}else{ACPL_RESET}
}

void srvc_getAppConfigSrField(ACPL *item, int (*getfunc)(AppSerialConfig *)){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	int serial_id;
	if(!acp_packGetCellI(item->buf, 2, &serial_id)){ACPL_RESET return;}
	AppConfig conf;
	if(!pmem_getAppConfig(&conf)){ACPL_RESET return;}
	int v;
	int result = 0;
	FOREACH_SERIAL(i)
		AppSerialConfig *serial = &conf.serial[i];
		if(serial_id == serial->id){
			v = getfunc(serial);
			result = 1;
			goto done;
		}
	}
	done:
	if(result && acp_buildPackIIII(item->buf, ACP_BUF_MAX_LENGTH, app.id, serial_id, v, result)){
		ACPL_PREP_SEND
		return;
	}
	ACPL_RESET
}

void acnf_getAppSerialRate(ACPL *item, HardwareSerial *serial){srvc_getAppConfigSrField(item, &APPCONFIGSR_GET_FIELD_FUNC(rate));}
void acnf_getAppSerialConfig(ACPL *item, HardwareSerial *serial){srvc_getAppConfigSrField(item, &APPCONFIGSR_GET_FIELD_FUNC(config));}
void acnf_getAppSerialKind(ACPL *item, HardwareSerial *serial){srvc_getAppConfigSrField(item, &APPCONFIGSR_GET_FIELD_FUNC(kind));}

void acnf_getAppId(ACPL *item, HardwareSerial *serial){srvc_getAppConfigFieldBr(item, &APPCONFIG_GET_FIELD_FUNC(id));}
void acnf_getAppState(ACPL *item, HardwareSerial *serial){srvc_getrAppFieldSF(item, &app_getStateStr);}
void acnf_getAppError(ACPL *item, HardwareSerial *serial){srvc_getrAppFieldSF(item, &app_getErrorStr);}

void srvc_setAppConfigField(ACPL *item, int (*checkfunc)(int ), void (*setfunc)(AppConfig *, int v)){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	int v;
	if(acp_packGetCellI(item->buf, 2, &v)){
		AppConfig conf;
		if(pmem_getAppConfig(&conf)){
			if(checkfunc(v)){
				setfunc(&conf, v);
				pmem_saveAppConfig(&conf);
			}
		}
	}
	ACPL_RESET
}

void srvc_setAppSerialConfigField(ACPL *item, int (*checkfunc)(int ), void (*setfunc)(AppSerialConfig *, int v)){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	int serial_id;
	if(!acp_packGetCellI(item->buf, 2, &serial_id)){
		goto done;
	}
	int v;
	if(!acp_packGetCellI(item->buf, 3, &v)){
		goto done;
	}
	if(!checkfunc(v)){
		goto done;
	}
	AppConfig conf;
	if(!pmem_getAppConfig(&conf)){
		goto done;
	}
	FOREACH_SERIAL(i)
		if(serial_id == conf.serial[i].id){
			setfunc(&conf.serial[i], v);
			pmem_saveAppConfig(&conf);
			goto done;
		}
	}
	done:
	ACPL_RESET
}

void acnf_setAppId(ACPL *item, HardwareSerial *serial){
	int v;
	if(acp_packGetCellI(item->buf, 1, &v)){
		AppConfig conf;
		if(pmem_getAppConfig(&conf)){
			if(appc_checkId(v)){
				app.id = v;
				conf.id = app.id;
				pmem_saveAppConfig(&conf);
			}
		}
	}
	ACPL_RESET
}

void acnf_appReset(ACPL *item, HardwareSerial *serial){
	if(!srvc_forThisApp(item)) {ACPL_RESET return;}
	app_reset(&app);
	ACPL_RESET
}

void acnf_setAppSerialConfig(ACPL *item, HardwareSerial *serial){srvc_setAppSerialConfigField(item, &appc_checkSerialConfig, &APPCONFIGSR_SET_FIELD_FUNC(config));}
void acnf_setAppSerialRate(ACPL *item, HardwareSerial *serial){srvc_setAppSerialConfigField(item, &appc_checkSerialRate, &APPCONFIGSR_SET_FIELD_FUNC(rate));}
void acnf_setAppSerialKind(ACPL *item, HardwareSerial *serial){srvc_setAppSerialConfigField(item, &serial_checkAppKind, &APPCONFIGSR_SET_FIELD_FUNC(kind));}


ACPLCommandNode acnodes[] = {
	{CMD_SET_CHANNEL_GOAL,				&acnf_setGoal},
	{CMD_GET_ID_EXISTS,					&acnf_getExists},
	{CMD_GET_CHANNEL_FTS,				&acnf_getFTS},
	{CMD_GETR_CHANNEL_STATE,			&acnf_getStateStr},
	{CMD_GETR_CHANNEL_ERROR,			&acnf_getErrorStr},
	{CMD_GETR_SEC_STATE,				&acnf_getSecureStateStr},
	
	{CMD_CHANNEL_START,					&acnf_start},
	{CMD_CHANNEL_STOP,					&acnf_stop},
	{CMD_CHANNEL_RELOAD,				&acnf_reload},
	
#ifdef SERIAL_INTERFACE_FULL

#if OUTPUT_MODE == OUT_PWM
	{CMD_SET_PWM_RESOLUTION,			&acnf_setPWMResolution},
	{CMD_SET_PWM_PERIOD,				&acnf_setPWMPeriod},
	{CMD_SET_PWM_PW_MIN,				&acnf_setPWMDCMin},
	{CMD_SET_PWM_PW_MAX,				&acnf_setPWMDCMax},
	{CMD_GET_PWM_RESOLUTION,			&acnf_getPWMResolution},
	{CMD_GET_PWM_PERIOD,				&acnf_getPWMPeriod},
	{CMD_GET_PWM_PW_MIN,				&acnf_getPWMDCMin},
	{CMD_GET_PWM_PW_MAX,				&acnf_getPWMDCMax},
	{CMD_GETR_PWM_RESOLUTION,			&acnf_getrPWMResolution},
	{CMD_GETR_PWM_PERIOD,				&acnf_getrPWMPeriod},
	{CMD_GETR_PWM_PW_MIN,				&acnf_getrPWMDCMin},
	{CMD_GETR_PWM_PW_MAX,				&acnf_getrPWMDCMax},
#else
#if OUTPUT_MODE == OUT_SERVO
	{CMD_SET_SERVO_PW_MIN,				&acnf_setServoPWMin},
	{CMD_SET_SERVO_PW_MAX,				&acnf_setServoPWMax},
	{CMD_SET_SERVO_IN_MIN,				&acnf_setServoInMin},
	{CMD_SET_SERVO_IN_MAX,				&acnf_setServoInMax},
	{CMD_GET_SERVO_PW_MIN,				&acnf_getServoPWMin},
	{CMD_GET_SERVO_PW_MAX,				&acnf_getServoPWMax},
	{CMD_GET_SERVO_IN_MIN,				&acnf_getServoInMin},
	{CMD_GET_SERVO_IN_MAX,				&acnf_getServoInMax},
	{CMD_GETR_SERVO_PW_MIN,				&acnf_getrServoPWMin},
	{CMD_GETR_SERVO_PW_MAX,				&acnf_getrServoPWMax},
	{CMD_GETR_SERVO_IN_MIN,				&acnf_getrServoInMin},
	{CMD_GETR_SERVO_IN_MAX,				&acnf_getrServoInMax},
#endif
#endif

	{CMD_SET_CHANNEL_ID,				&acnf_setId},
	{CMD_SET_SEC_TM,					&acnf_setSecureTimeout},
	{CMD_SET_SEC_OUT,					&acnf_setSecureOut},
	{CMD_SET_SEC_ENABLE,				&acnf_setSecureEnable},
	{CMD_GET_SEC_TM,					&acnf_getSecureTimeout},
	{CMD_GET_SEC_OUT,					&acnf_getSecureOut},
	{CMD_GET_SEC_ENABLE,				&acnf_getSecureEnable},
	{CMD_GETR_SEC_ENABLE,				&acnf_getrSecureEnable},
	{CMD_GETR_SEC_TM,					&acnf_getrSecureTimeout},
	{CMD_GETR_SEC_OUT,					&acnf_getrSecureOut},
	
	{CMD_GET_APP_VARIABLES,				&acnf_getAppVar},
	
	{CMD_GETR_CHANNEL_ENABLE,			&acnf_getrEnable},

	{CMD_SET_APP_ID,					&acnf_setAppId},
	{CMD_SET_APP_CHANNEL_ID_FIRST,		&acnf_setIdFirst},
	{CMD_GET_APP_CHANNEL_ID_FIRST,		&acnf_getIdFirst},
	{CMD_APP_RESET,						&acnf_appReset},
	
	{CMD_GET_APP_ID,					&acnf_getAppId},
	
	{CMD_GET_APP_STATE,					&acnf_getAppState},
	{CMD_GET_APP_ERROR,					&acnf_getAppError},
	
	{CMD_SET_APP_SERIAL_KIND,			&acnf_setAppSerialKind},
	{CMD_SET_APP_SERIAL_RATE,			&acnf_setAppSerialRate},
	{CMD_SET_APP_SERIAL_CONFIG,			&acnf_setAppSerialConfig},
	
	{CMD_GET_APP_SERIAL_RATE,			&acnf_getAppSerialRate},
	{CMD_GET_APP_SERIAL_CONFIG,			&acnf_getAppSerialConfig},
	{CMD_GET_APP_SERIAL_KIND,			&acnf_getAppSerialKind}
#endif

	{CMD_GET_CHANNEL_GOAL,				&acnf_getGoal},
	{CMD_GETR_CHANNEL_GOAL,				&acnf_getrGoal}
};

#define ACPL_CNODE_COUNT (sizeof acnodes / sizeof acnodes[0])
