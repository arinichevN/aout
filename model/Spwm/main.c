#include "main.h"

static void spwm_calc_single_time (Spwm *self) {
	//next lines shortly: self->single_time = ( self->duty_cycle_max - self->duty_cycle_min ) / self->resolution;
	double dif = (double)self->duty_cycle_max - (double)self->duty_cycle_min;
	self->single_time = dif / (double) self->resolution;
}

static void spwm_calc_delays(Spwm *self){
	unsigned long dc = (unsigned long) (self->single_time * (double)self->duty_cycle);
	self->delay_idle = self->period - dc - self->duty_cycle_min;
	self->delay_busy = dc + self->duty_cycle_min;
}

static int spwm_setDutyCycle (Spwm *self, unsigned long v) {
	if(v == self->duty_cycle){
		return 0;
	}
	if (v > self->resolution) {
		self->duty_cycle=self->resolution;
	}else{
		self->duty_cycle = v;
	}
	spwm_calc_delays(self);
	//printd("output: ");printdln(v);
	//printd("delay_idle: ");printdln(self->delay_idle);
	//printd("delay_busy: ");printdln(self->delay_busy);printdln(" ");
	//self->toggle_time = millis();
	return 1;
}

static void spwm_BUSY (Spwm *self, double goal);

static void spwm_IDLE (Spwm *self, double goal){
	spwm_setDutyCycle (self, (unsigned long) goal);
	unsigned long now = millis();
	if (now > self->toggle_time) {
		self->toggle_time = now + self->delay_busy;
		if(self->delay_busy > 0){
		   digitalWrite(self->pin, SPWM_PIN_BUSY);
		}
		self->control = spwm_BUSY;
	}
}

static void spwm_BUSY (Spwm *self, double goal){
	spwm_setDutyCycle (self, (unsigned long) goal);
	unsigned long now = millis();
	if (now > self->toggle_time) {
		self->toggle_time = now + self->delay_idle;
		if(self->delay_idle > 0){
		   digitalWrite(self->pin, SPWM_PIN_IDLE);
		}
		self->control = spwm_IDLE;
	}
}

void spwm_setParam(Spwm *self, const SpwmParam *param){
	self->resolution = param->resolution;
	self->period = param->period;
	self->duty_cycle_min = param->duty_cycle_min;
	self->duty_cycle_max = param->duty_cycle_max;
}

void spwm_setPin(Spwm *self, int pin){
	self->pin = pin;
	pinMode(self->pin, OUTPUT);
	digitalWrite(self->pin, SPWM_PIN_IDLE);
}

int spwm_getPin(Spwm *self){
	return self->pin;
}

static void spwm_begin (void *vself) {
	Spwm *self = (Spwm *) vself;
	spwm_setDutyCycle (self, 0);
	unsigned long  now = millis();
	self->toggle_time = now;
	spwm_calc_single_time (self);
	spwm_calc_delays(self);
	self->control = spwm_IDLE;
}

static void spwm_start (void *vself) {
	Spwm *self = (Spwm *) vself;
	spwm_setDutyCycle (self, 0);
	unsigned long  now = millis();
	self->toggle_time = now;
	self->control = spwm_IDLE;
}

static void spwm_stop(void *vself){
	Spwm *self = (Spwm *) vself;
	digitalWrite(self->pin, SPWM_PIN_IDLE);
	self->control = spwm_IDLE;
}

static void spwm_free(void *vself){
	Spwm *self = (Spwm *) vself;
	free(self);
}

static int spwm_normalizeInput(void *vself, double *v){
	Spwm *self = (Spwm *) vself;
	if(*v > (double) self->resolution) {
		*v = (double) self->resolution;
		return 1;
	}
	if(*v < 0.0) {
		*v = 0.0;
		return 1;
	}
	return 0;
}

static void spwm_control(void *vself, double goal){
	Spwm *self = (Spwm *) vself;
	self->control(self, goal);
}

static double spwm_getInitialGoal(void *vself){
	return 0.0;
}

static void spwm_buildInterfaces(Spwm *self){
	self->im_device.self = self;
	self->im_device.begin = spwm_begin;
	self->im_device.start = spwm_start;
	self->im_device.control = spwm_control;
	self->im_device.normalizeInput = spwm_normalizeInput;
	self->im_device.getInitialGoal = spwm_getInitialGoal;
	self->im_device.stop = spwm_stop;
	self->im_device.free = spwm_free;
}

Spwm *spwm_new(){
	size_t sz = sizeof (Spwm);
	Spwm *out = (Spwm *) malloc(sz);
	if(out == NULL){ 
		printdln("spwm_new: failed");
	}else{
		spwm_buildInterfaces(out);
	}
	printd("spwm_new: "); printd(sz); printdln(" bytes allocated");
	return out;
}
