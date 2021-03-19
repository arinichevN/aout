#include "main.h"

static void channel_RUN(Channel *self);
static void channel_OFF(Channel *self);
static void channel_INIT(Channel *self);

state_t channel_getState(Channel *self){
	if(self->control == channel_OFF) {
		return STATE_OFF;
	} else if(self->control == channel_RUN) {
		return STATE_RUN;
	} else if(self->control == channel_INIT) {
		return STATE_INIT;
	}
	return STATE_UNKNOWN;
}

err_t channel_getError(Channel *self){
	return self->error_id;
}

static void channel_buildInterfaces(Channel *self){
	;
}

static err_t channel_setParam(Channel *self, size_t ind){
	self->ind = ind;
	ChannelParam param;
	if(!pmem_getChannelParam(&param, ind)){
		printdln("   failed to get channel from NVRAM");
		return ERROR_NVRAM_READ;
	}
	err_t r = channelParam_check(&param);
	if(r != ERROR_NO){
		return r;
	}
	self->id = param.id;
	self->pin = param.pin;
	self->device_kind = param.device_kind;
	idev_begin(&self->idev);
	self->device = &self->idev.im_device;
	switch(self->device_kind){
		case DEVICE_KIND_HPWM:{
			Hpwm *device = hpwm_new();
			if(device != NULL){
				hpwm_setParam(device, &param.hpwm);
				hpwm_setPin(device, self->pin);
				self->device = &device->im_device;
			}}
			break;
		case DEVICE_KIND_SPWM:{
			Spwm *device = spwm_new();
			if(device != NULL){
				spwm_setParam(device, &param.spwm);
				spwm_setPin(device, self->pin);
				self->device = &device->im_device;
			}}
			break;
		default:
			return ERROR_DEVICE_KIND;
	}
	secure_setParam(&self->secure, &param.secure);
	return ERROR_NO;
}

void channel_begin(Channel *self, size_t ind){
	printd("beginning channel ");printd(ind); printdln(":");
	channel_buildInterfaces(self);
	self->error_id = channel_setParam(self, ind);
	self->control = channel_INIT;
	printd("\tid: ");printdln(self->id);
	printd("\n");
}


void channel_free(Channel *self){
	self->device->free(self->device->self);
}

int channel_start(Channel *self){
	if(self->control == channel_OFF){
		printd("starting channel ");printd(self->ind);printdln(":");
		self->device->start(self->device->self);
		secure_start(&self->secure);
		self->control = channel_RUN;
		return 1;
	}
	return 0;
}

int channel_stop(Channel *self){
	printd("stopping channel ");printdln(self->ind);
	self->device->stop(self->device->self);
	self->goal = self->device->getInitialGoal(self->device->self);
	secure_stop(&self->secure);
	self->error_id = ERROR_NO;
	self->control = channel_OFF;
	return 1;
}

int channel_disconnect(Channel *self){
	printd("disconnecting channel ");printdln(self->ind);
	self->device->stop(self->device->self);
	self->goal = self->device->getInitialGoal(self->device->self);
	secure_stop(&self->secure);
	self->error_id = ERROR_NO;
	self->control = channel_OFF;
	return 1;
}

int channel_reset(Channel *self){
	printd("restarting channel ");printd(self->ind); printdln(":");
	self->device->stop(self->device->self);
	self->goal = self->device->getInitialGoal(self->device->self);
	secure_stop(&self->secure);
	channel_free(self);
	channel_begin(self, self->ind);
	return 1;
}


static void channel_RUN(Channel *self){
	SECURE_CONTROL(&self->secure, &self->goal);
	self->device->control(self->device->self, self->goal);
}

static void channel_OFF(Channel *self){
	;
}

static void channel_INIT(Channel *self){
	self->device->begin(self->device->self);
	secure_begin(&self->secure);
	self->goal = self->device->getInitialGoal(self->device->self);
	self->control = channel_OFF;
}

void channel_setGoal(Channel *self, double v){
	if(self->control != channel_RUN) return;
	self->device->normalizeInput(self->device->self, &v);
	self->goal = v;
	secure_touch(&self->secure);
}


#ifdef USE_AOIDS
#include "aoid.c"

void channel_buildAoids(Channel *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	channelAoid_build(&self->aoid, next_oid, parent_oid, self, id);
}

#endif

#ifdef USE_NOIDS
#include "noid.c"
#endif
