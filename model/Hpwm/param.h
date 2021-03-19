#ifndef MODEL_HPWM_PARAM_H
#define MODEL_HPWM_PARAM_H

#include "../../lib/common.h"

typedef struct {
	unsigned long pw_min;//pulse width, microseconds
	unsigned long pw_max;//pulse width, microseconds
	double in_min;
	double in_max;
} HpwmParam;

extern err_t hpwmParam_check(const HpwmParam *self);

#endif 
