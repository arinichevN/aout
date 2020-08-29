#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H

#if OUTPUT_MODE == OUT_PWM
#include "../pwm.h"
#else
#if OUTPUT_MODE == OUT_SERVO
#include "../servo.h"
#endif
#endif

#include "../../util/dstructure.h"
#include "../../util/common.h"
#include "../secure.h"

#include "../../app/serial.h"


struct channel_st {
#if OUTPUT_MODE == OUT_PWM
	PWM pwm;
#else
#if OUTPUT_MODE == OUT_SERVO
	Servow servo;
#endif
#endif
	Secure secure;
	int id;//for external control
	size_t ind;//index in array
	double goal;
	int device_kind;
	int error_id;
	int enable;
	void (*control) (struct channel_st *);
	struct channel_st *next;
};
typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel)
#define CHANNEL_SAVE_FIELD(F) PmemChannel pchannel;	if(pmem_getPChannel(&pchannel, item->ind)){pchannel.F = item->F; pmem_savePChannel(&pchannel, item->ind);}
#define CHANNEL_FUN_GET(param) channel_get_ ## param

#endif 
