#include "aoid.h"

static void channel_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_START:
			channel_start(self);
			acpls_reset(server);
			return;
		case CMD_AOID_STOP:
			channel_stop(self);
			acpls_reset(server);
			return;
		case CMD_AOID_RESET:
			channel_reset(self);
			acpls_reset(server);
			return;
		case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			aoidServer_sendSupportedSSR(oid, server);
			return;
	}
	acpls_reset(server);
}

static void channel_serveRequestNone(void *vself, Aoid *oid, void *vserver, int command) {
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			aoidServer_sendSupportedNone(oid, server);
			return;
	}
	acpls_reset(server);
}

static void channel_serveAoidRequestGGS(void *vself, Aoid *oid, void *vserver, int command, void (*sendRamParam)(Aoid *, Acpls *, Channel *), void (*sendNvramParam)(Aoid *, Acpls *, const ChannelParam *, yn_t), int (*setNvramParam)(Acpls *, ChannelParam *)){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			sendRamParam(oid, server, self);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:{
				yn_t success = NO;
				ChannelParam param;
				memset(&param, 0, sizeof param);
				if(pmem_getChannelParam(&param, self->ind)){
					success = YES;
				}
				sendNvramParam(oid, server, &param, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				ChannelParam param;
				if(!pmem_getChannelParam(&param, self->ind)){
					acpls_reset(server);
					return;
				}
				if(!setNvramParam(server, &param)){
					acpls_reset(server);
					return;
				}
				if(channelParam_check(&param) == ERROR_NO){
					pmem_saveChannelParam(&param, self->ind);
				}
			}
			acpls_reset(server);
			return;
		case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			aoidServer_sendSupportedGGS(oid, server);
			return;
	}
	acpls_reset(server);
}

static void channel_sendRamParamId(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->id, YES);}
static void channel_sendNvramParamId(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->id, success);}
static int channel_setNvramParamId(Acpls *server, ChannelParam *param){int v;	if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){	return 0;	}	param->id = v;	return 1;}
static void channel_serveAoidRequestId(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamId, channel_sendNvramParamId, channel_setNvramParamId);}

static void channel_sendRamParamPin(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->pin, YES);}
static void channel_sendNvramParamPin(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->pin, success);}
static int channel_setNvramParamPin(Acpls *server, ChannelParam *param){int v;	if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){	return 0;	}	param->pin = v;	return 1;}
static void channel_serveAoidRequestPin(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamPin, channel_sendNvramParamPin, channel_setNvramParamPin);}

static void channel_sendRamParamDeviceKind(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->device_kind, YES);}
static void channel_sendNvramParamDeviceKind(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->device_kind, success);}
static int channel_setNvramParamDeviceKind(Acpls *server, ChannelParam *param){int v;	if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->device_kind = (dk_t) v; return 1;}
static void channel_serveAoidRequestDeviceKind(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamDeviceKind, channel_sendNvramParamDeviceKind, channel_setNvramParamDeviceKind);}


static void channel_sendRamParamHpwmPwMin(Aoid *oid, Acpls *server, Channel *channel){unsigned long v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_HPWM){Hpwm *device = (Hpwm *) channel->device->self; v = device->pw_min; success = YES;} aoidServer_sendUlI(oid, server, v, success);}
static void channel_sendNvramParamHpwmPwMin(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->hpwm.pw_min, success);}
static int channel_setNvramParamHpwmPwMin(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->hpwm.pw_min = v; return 1;}
static void channel_serveAoidRequestHpwmPwMin(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamHpwmPwMin, channel_sendNvramParamHpwmPwMin, channel_setNvramParamHpwmPwMin);}

static void channel_sendRamParamHpwmPwMax(Aoid *oid, Acpls *server, Channel *channel){unsigned long v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_HPWM){Hpwm *device = (Hpwm *) channel->device->self; v = device->pw_max; success = YES;} aoidServer_sendUlI(oid, server, v, success);}
static void channel_sendNvramParamHpwmPwMax(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->hpwm.pw_max, success);}
static int channel_setNvramParamHpwmPwMax(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->hpwm.pw_max = v; return 1;}
static void channel_serveAoidRequestHpwmPwMax(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamHpwmPwMax, channel_sendNvramParamHpwmPwMax, channel_setNvramParamHpwmPwMax);}

static void channel_sendRamParamHpwmInMin(Aoid *oid, Acpls *server, Channel *channel){double v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_HPWM){Hpwm *device = (Hpwm *) channel->device->self; v = device->in_min; success = YES;} aoidServer_sendFI(oid, server, v, success);}
static void channel_sendNvramParamHpwmInMin(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendFI(oid, server, param->hpwm.in_min, success);}
static int channel_setNvramParamHpwmInMin(Acpls *server, ChannelParam *param){double v; if(!acp_packGetCellF(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->hpwm.in_min = v; return 1;}
static void channel_serveAoidRequestHpwmInMin(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamHpwmInMin, channel_sendNvramParamHpwmInMin, channel_setNvramParamHpwmInMin);}

static void channel_sendRamParamHpwmInMax(Aoid *oid, Acpls *server, Channel *channel){double v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_HPWM){Hpwm *device = (Hpwm *) channel->device->self; v = device->in_max; success = YES;} aoidServer_sendFI(oid, server, v, success);}
static void channel_sendNvramParamHpwmInMax(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendFI(oid, server, param->hpwm.in_max, success);}
static int channel_setNvramParamHpwmInMax(Acpls *server, ChannelParam *param){double v; if(!acp_packGetCellF(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->hpwm.in_max = v; return 1;}
static void channel_serveAoidRequestHpwmInMax(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamHpwmInMax, channel_sendNvramParamHpwmInMax, channel_setNvramParamHpwmInMax);}


static void channel_sendRamParamSpwmResolution(Aoid *oid, Acpls *server, Channel *channel){unsigned long v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_SPWM){Spwm *device = (Spwm *) channel->device->self; v = device->resolution; success = YES;} aoidServer_sendUlI(oid, server, v, success);}
static void channel_sendNvramParamSpwmResolution(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->spwm.resolution, success);}
static int channel_setNvramParamSpwmResolution(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->spwm.resolution = v; return 1;}
static void channel_serveAoidRequestSpwmResolution(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamSpwmResolution, channel_sendNvramParamSpwmResolution, channel_setNvramParamSpwmResolution);}

static void channel_sendRamParamSpwmPeriod(Aoid *oid, Acpls *server, Channel *channel){unsigned long v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_SPWM){Spwm *device = (Spwm *) channel->device->self; v = device->period; success = YES;} aoidServer_sendUlI(oid, server, v, success);}
static void channel_sendNvramParamSpwmPeriod(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->spwm.period, success);}
static int channel_setNvramParamSpwmPeriod(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->spwm.period = v; return 1;}
static void channel_serveAoidRequestSpwmPeriod(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamSpwmPeriod, channel_sendNvramParamSpwmPeriod, channel_setNvramParamSpwmPeriod);}

static void channel_sendRamParamSpwmDcmin(Aoid *oid, Acpls *server, Channel *channel){unsigned long v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_SPWM){Spwm *device = (Spwm *) channel->device->self; v = device->duty_cycle_min; success = YES;} aoidServer_sendUlI(oid, server, v, success);}
static void channel_sendNvramParamSpwmDcmin(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->spwm.duty_cycle_min, success);}
static int channel_setNvramParamSpwmDcmin(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->spwm.duty_cycle_min = v; return 1;}
static void channel_serveAoidRequestSpwmDcmin(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamSpwmDcmin, channel_sendNvramParamSpwmDcmin, channel_setNvramParamSpwmDcmin);}

static void channel_sendRamParamSpwmDcmax(Aoid *oid, Acpls *server, Channel *channel){unsigned long v = 0; yn_t success = NO; if(channel->device_kind == DEVICE_KIND_SPWM){Spwm *device = (Spwm *) channel->device->self; v = device->duty_cycle_max; success = YES;} aoidServer_sendUlI(oid, server, v, success);}
static void channel_sendNvramParamSpwmDcmax(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->spwm.duty_cycle_max, success);}
static int channel_setNvramParamSpwmDcmax(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->spwm.duty_cycle_max = v; return 1;}
static void channel_serveAoidRequestSpwmDcmax(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamSpwmDcmax, channel_sendNvramParamSpwmDcmax, channel_setNvramParamSpwmDcmax);}

static void channel_sendRamParamSecureOut(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendFI(oid, server, channel->secure.out, YES);}
static void channel_sendNvramParamSecureOut(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendFI(oid, server, param->secure.out, success);}
static int channel_setNvramParamSecureOut(Acpls *server, ChannelParam *param){double v; if(!acp_packGetCellF(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->secure.out = v; return 1;}
static void channel_serveAoidRequestSecureOut(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamSecureOut, channel_sendNvramParamSecureOut, channel_setNvramParamSecureOut);}

static void channel_sendRamParamSecureTimeout(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendUlI(oid, server, channel->secure.tmr.interval, YES);}
static void channel_sendNvramParamSecureTimeout(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->secure.timeout, success);}
static int channel_setNvramParamSecureTimeout(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->secure.timeout = v; return 1;}
static void channel_serveAoidRequestSecureTimeout(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamSecureTimeout, channel_sendNvramParamSecureTimeout, channel_setNvramParamSecureTimeout);}

static void channel_sendRamParamSecureEnable(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->secure.enable, YES);}
static void channel_sendNvramParamSecureEnable(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->secure.enable, success);}
static int channel_setNvramParamSecureEnable(Acpls *server, ChannelParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->secure.enable = (yn_t) v; return 1;}
static void channel_serveAoidRequestSecureEnable(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamSecureEnable, channel_sendNvramParamSecureEnable, channel_setNvramParamSecureEnable);}



void hpwmAoid_build(HpwmAoid *self,  Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,							DESCR,									FUNCTION, 								DATA,	ID)
	AOID_SET_PARAM(&self->main,					&self->pw_min,				parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_HPWM,					channel_serveRequestNone,				vchannel,	*id)
	AOID_SET_PARAM(&self->pw_min,				&self->pw_max,				&self->main,	AOID_KIND_TIMEUS_PARAM,			AOID_DESCRIPTION_PULSE_WIDTH_MIN_US,	channel_serveAoidRequestHpwmPwMin,		vchannel,	*id)
	AOID_SET_PARAM(&self->pw_max,				&self->in_min,				&self->main,	AOID_KIND_TIMEUS_PARAM,			AOID_DESCRIPTION_PULSE_WIDTH_MAX_US,	channel_serveAoidRequestHpwmPwMax,		vchannel,	*id)
	AOID_SET_PARAM(&self->in_min,				&self->in_max,				&self->main,	AOID_KIND_FLOAT_PARAM,			AOID_DESCRIPTION_INPUT_MIN,				channel_serveAoidRequestHpwmInMin,		vchannel,	*id)
	AOID_SET_PARAM(&self->in_max,				next,						&self->main,	AOID_KIND_FLOAT_PARAM,			AOID_DESCRIPTION_INPUT_MAX,				channel_serveAoidRequestHpwmInMax,		vchannel,	*id)
}

void spwmAoid_build(SpwmAoid *self,  Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,							DESCR,									FUNCTION, 								DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->resolution,			parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_SPWM,					channel_serveRequestNone,				vchannel,	*id)
	AOID_SET_PARAM(&self->resolution,			&self->period,				&self->main,	AOID_KIND_UINT16_PARAM,			AOID_DESCRIPTION_RESOLUTION,			channel_serveAoidRequestSpwmResolution,	vchannel,	*id)
	AOID_SET_PARAM(&self->period,				&self->duty_cycle_min,		&self->main,	AOID_KIND_TIMEMS_PARAM,			AOID_DESCRIPTION_PERIOD_MS,				channel_serveAoidRequestSpwmPeriod,		vchannel,	*id)
	AOID_SET_PARAM(&self->duty_cycle_min,		&self->duty_cycle_max,		&self->main,	AOID_KIND_TIMEMS_PARAM,			AOID_DESCRIPTION_DUTY_CYCLE_MIN_MS,		channel_serveAoidRequestSpwmDcmin,		vchannel,	*id)
	AOID_SET_PARAM(&self->duty_cycle_max,		next,						&self->main,	AOID_KIND_TIMEMS_PARAM,			AOID_DESCRIPTION_DUTY_CYCLE_MAX_MS,		channel_serveAoidRequestSpwmDcmax,		vchannel,	*id)
}

void secureAoid_build(SecureAoid *self,  Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,							DESCR,									FUNCTION, 									DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->out,					parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_SECURE,				channel_serveRequestNone,					vchannel,	*id)
	AOID_SET_PARAM(&self->out,					&self->timeout,				&self->main,	AOID_KIND_FLOAT_PARAM,			AOID_DESCRIPTION_OUTPUT,				channel_serveAoidRequestSecureOut,			vchannel,	*id)
	AOID_SET_PARAM(&self->timeout,				&self->enable,				&self->main,	AOID_KIND_TIMEMS_PARAM,			AOID_DESCRIPTION_TIMEOUT,				channel_serveAoidRequestSecureTimeout,		vchannel,	*id)
	AOID_SET_PARAM(&self->enable,				next,						&self->main,	AOID_KIND_YN_PARAM,				AOID_DESCRIPTION_ENABLE,				channel_serveAoidRequestSecureEnable,		vchannel,	*id)
}

void channelAoid_build(ChannelAoid *self, Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,							DESCR,									FUNCTION, 								DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->id,					parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_CHANNEL,				channel_serveAoidRequestSelf,			vchannel,	*id)
	AOID_SET_PARAM(&self->id,					&self->pin,					&self->main,	AOID_KIND_ID_PARAM,				AOID_DESCRIPTION_ID,					channel_serveAoidRequestId,				vchannel,	*id)
	AOID_SET_PARAM(&self->pin,					&self->device_kind,			&self->main,	AOID_KIND_UINT8_PARAM,			AOID_DESCRIPTION_PIN,					channel_serveAoidRequestPin,			vchannel,	*id)
	AOID_SET_PARAM(&self->device_kind,			&self->hpwm.main,			&self->main,	AOID_KIND_DEVICE_KIND_PARAM,	AOID_DESCRIPTION_DEVICE_KIND,			channel_serveAoidRequestDeviceKind,		vchannel,	*id)
	hpwmAoid_build(&self->hpwm,					&self->spwm.main, 			&self->main,																													vchannel,	id);
	spwmAoid_build(&self->spwm, 				&self->secure.main,			&self->main,																													vchannel,	id);
	secureAoid_build(&self->secure, 			next, 						&self->main,																													vchannel,	id);
}
