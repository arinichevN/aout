#ifndef MODEL_IDLE_DEVICE_H
#define MODEL_IDLE_DEVICE_H

typedef struct {
	iDevice im_device;
} IdleDevice;

extern void idev_new(IdleDevice *self);

#endif
