#include "main.h"

void secure_DO (Secure *self, double *v){
	*v = self->out;
}

void secure_WAIT (Secure *self, double *v){
	if(ton(&self->tmr)){
		self->control = secure_DO;
		*v = self->out;
	}
}

void secure_OFF (Secure *self, double *v){
	;
}

void secure_INIT (Secure *self, double *v){
	ton_reset(&self->tmr);
	self->control = secure_OFF;
    if(self->enable == YES){
		self->control = secure_WAIT;
    }
}

void secure_begin (Secure *self) {
	self->control = secure_INIT;
}

void secure_start (Secure *self) {
	self->control = secure_INIT;
}

void secure_stop (Secure *self) {
	self->control = secure_OFF;
}

void secure_setParam (Secure *self, const SecureParam *param) {
	ton_setInterval(&self->tmr, param->timeout);
	self->out = param->out;
	self->enable = param->enable;
}

void secure_touch (Secure *self) {
	self->control = secure_INIT;
}

const char *secure_getStateStr(Secure *self){
	if(self->control == secure_INIT)		return "INI";
	else if(self->control == secure_DO)		return "SEC";
	else if(self->control == secure_OFF)	return "OFF";
	else if(self->control == secure_WAIT)	return "WAIT";
	return " ? ";
}

void secure_setEnable(Secure *self, int v){
	if(v == YES){
		if(self->control == secure_OFF){
			self->control = secure_INIT;
		}
	}else{
	    self->control = secure_OFF;
	}
}
