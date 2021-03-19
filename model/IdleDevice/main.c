#include "main.h"

static void idev_control(void *vself, double goal){
	;
}

static void idev_stop(void *vself){
	;
}

static void idev_free(void *vself){
	;
}

static void idev_begin(void *vself){
	;
}

static void idev_start(void *vself){
	;
}

static int idev_normalizeInput(void *vself, double *v){
	return 0;
}

static double idev_getInitialGoal(void *vself){
	return 0.0;
}

static void idev_buildInterfaces(IdleDevice *self){
	self->im_device.self = self;
	self->im_device.begin = idev_begin;
	self->im_device.start = idev_start;
	self->im_device.control = idev_control;
	self->im_device.normalizeInput = idev_normalizeInput;
	self->im_device.getInitialGoal = idev_getInitialGoal;
	self->im_device.stop = idev_stop;
	self->im_device.free = idev_free;
}

void idev_begin(IdleDevice *self){
	idev_buildInterfaces(self);
}
