#include "main.h"

static void idev_control(void *vself, double goal){
	;
}

static void idev_stop(void *vself){
	;
}

static void idev_free(void *vself){
	IdleDevice *self = (IdleDevice *) vself;
	free(self);
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

IdleDevice *idev_new(){
	size_t sz = sizeof (IdleDevice);
	IdleDevice *out = (IdleDevice *) malloc(sz);
	if(out == NULL){
		printdln("idev_new: failed");
	} else{
		idev_buildInterfaces(out);
	}
	printd("idev_new: "); printd(sz); printdln(" bytes allocated");
	return out;
}
