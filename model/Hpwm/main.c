#include "main.h"

#define HPWM_IMPOSSIBLE_GOAL -0.00001

static int hpwm_map(double x, double in_min, double in_max,  unsigned long out_min,  unsigned long out_max){
  return (int) (x - in_min) * ((double)out_max - (double)out_min) / (in_max - in_min) + (double)out_min;
}

static void hpwm_control(void *vself, double goal){
	Hpwm *self = (Hpwm *) vself;
	if(self->last_goal == goal) return;
	int tms = hpwm_map(goal, self->in_min, self->in_max, self->pw_min, self->pw_max);
	self->device.writeMicroseconds(tms);printd(" servo pw: ");printd(tms);printd("\n");
	self->last_goal = goal;
}

void hpwm_setParam(Hpwm *self, const HpwmParam *param){
	self->pw_min = param->pw_min;
	self->pw_max = param->pw_max;
	self->in_min = param->in_min;
	self->in_max = param->in_max;
}

void hpwm_setPin (Hpwm *self, int pin){
	self->pin = pin;
	self->device.attach(pin, self->pw_min, self->pw_max);
}

static void hpwm_begin (void *vself){
	Hpwm *self = (Hpwm *) vself;
	self->last_goal = HPWM_IMPOSSIBLE_GOAL;
}

static void hpwm_start (void *vself){
	Hpwm *self = (Hpwm *) vself;
	self->device.attach(self->pin, self->pw_min, self->pw_max);
	double goal = self->in_min;
	int tms = hpwm_map(goal, self->in_min, self->in_max, self->pw_min, self->pw_max);
	self->device.writeMicroseconds(tms);printd(" servo pw: ");printd(tms);printd("\n");
	self->last_goal = goal;
}

static void hpwm_stop(void *vself){
	Hpwm *self = (Hpwm *) vself;
	self->device.detach();
}

static void hpwm_free(void *vself){
	Hpwm *self = (Hpwm *) vself;
	self->device.detach();
	free(self);
}

static int hpwm_normalizeInput(void *vself, double *v){
	Hpwm *self = (Hpwm *) vself;
	if(*v > self->in_max) {
		*v = self->in_max;
		return 1;
	}
	if(*v < self->in_min) {
		*v = self->in_min;
		return 1;
	}
	return 0;
}

static double hpwm_getInitialGoal(void *vself){
	Hpwm *self = (Hpwm *) vself;
	return self->in_min;
}

static void hpwm_buildInterfaces(Hpwm *self){
	self->im_device.self = self;
	self->im_device.begin = hpwm_begin;
	self->im_device.start = hpwm_start;
	self->im_device.control = hpwm_control;
	self->im_device.normalizeInput = hpwm_normalizeInput;
	self->im_device.getInitialGoal = hpwm_getInitialGoal;
	self->im_device.stop = hpwm_stop;
	self->im_device.free = hpwm_free;
}

Hpwm *hpwm_new(){
	size_t sz = sizeof (Hpwm);
	Hpwm *out = (Hpwm *) malloc(sz);
	if(out == NULL){
		printdln("hpwm_new: failed");
	} else{
		hpwm_buildInterfaces(out);
	}
	printd("hpwm_new: "); printd(sz); printdln(" bytes allocated");
	return out;
}
#undef HPWM_IMPOSSIBLE_GOAL
