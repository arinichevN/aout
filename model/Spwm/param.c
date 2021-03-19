#include "param.h"

err_t spwmParam_check(const SpwmParam *self){
	if(self->duty_cycle_max < self->duty_cycle_min){
		return ERROR_PWM;
	}
	if(self->duty_cycle_max > self->period){
		return ERROR_PWM;
	}
	return ERROR_NO;
}
