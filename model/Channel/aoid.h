#ifndef MODEL_CHANNEL_AOID_H
#define MODEL_CHANNEL_AOID_H

#include "../../lib/common.h"

typedef struct {
	Aoid main;
	Aoid pw_min;
	Aoid pw_max;
	Aoid in_min;
	Aoid in_max;
} HpwmAoid;

typedef struct {
	Aoid main;
	Aoid resolution;
	Aoid period;
	Aoid duty_cycle_min;
	Aoid duty_cycle_max;
} SpwmAoid;

typedef struct {
	Aoid main;
	Aoid out;
	Aoid timeout;
	Aoid enable;
} SecureAoid;

typedef struct {
	Aoid main;
	Aoid id;
	Aoid pin;
	Aoid device_kind;
	HpwmAoid hpwm;
	SpwmAoid spwm;
	SecureAoid secure;
} ChannelAoid;

extern void channelAoid_build(ChannelAoid *self, Aoid *next, Aoid *parent, size_t *id, void *vchannel);

#endif 
