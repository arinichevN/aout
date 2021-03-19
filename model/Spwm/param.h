#ifndef MODEL_SPWM_PARAM_H
#define MODEL_SPWM_PARAM_H

#include "../../lib/common.h"

typedef struct {
	unsigned long resolution;
	unsigned long period;
	unsigned long duty_cycle_min; 
	unsigned long duty_cycle_max;
} SpwmParam;

extern err_t spwmParam_check(const SpwmParam *self);

#endif 
