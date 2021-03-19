#ifndef MODEL_CHANNEL_PARAM_H
#define MODEL_CHANNEL_PARAM_H

#include "../Hpwm/param.h"
#include "../Spwm/param.h"
#include "../Secure/param.h"
#include "../../lib/common.h"

typedef struct {
	int id;
	int pin;
	dk_t device_kind;
	HpwmParam hpwm;
	SpwmParam spwm;
	SecureParam secure;
} ChannelParam;

extern err_t channelParam_check(const ChannelParam *self);

#endif 
