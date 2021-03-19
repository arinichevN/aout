#include "param.h"

err_t channelParam_check(const ChannelParam *self){
	switch(self->device_kind){
		case DEVICE_KIND_HPWM: case DEVICE_KIND_SPWM:
			break;
		default:
			return ERROR_DEVICE_KIND;
	}
	err_t r = hpwmParam_check(&self->hpwm);
	if(r != ERROR_NO){
		return r;
	}
	r = spwmParam_check(&self->spwm);
	if(r != ERROR_NO){
		return r;
	}
	r = secureParam_check(&self->secure);
	if(r != ERROR_NO){
		return r;
	}
	return ERROR_NO;
}
