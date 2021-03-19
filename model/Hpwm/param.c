#include "param.h"

err_t hpwmParam_check(const HpwmParam *self){
	if(self->pw_min > self->pw_max){
		return ERROR_PWM;
	}
	if(self->in_min > self->in_max){
		return ERROR_PWM;
	}
	return ERROR_NO;
}
